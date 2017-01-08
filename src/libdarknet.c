#include <stdlib.h>
#include <string.h>

#include "libdarknet.h"

#include "network.h"
#include "parser.h"
#include "region_layer.h"
#include "stb_image.h"
#include "utils.h"


typedef struct libdarknet_detector
{
  network net;
  box *boxes;
  float **probs;
  ssize_t nfound;
  libdarknet_item *items;
  ssize_t nitems;
} *libdarknet_detector_t;


libdarknet_detector_t
libdarknet_new_detector (const char *cfg, const char *weights)
{
  libdarknet_detector_t d;

  d = malloc (sizeof (struct libdarknet_detector));
  d->net = parse_network_cfg (cfg);

  load_weights (&d->net, weights);
  set_batch_network (&d->net, 1);

  d->nitems = 30;               // Will realloc() if needed
  d->items = malloc (d->nitems * sizeof (libdarknet_item));

  layer *l = &d->net.layers[d->net.n - 1];
  int s = l->w * l->h * l->n;
  d->boxes = calloc (s, sizeof (box));
  d->probs = calloc (s, sizeof (float *));
  for (int i = 0; i < s; ++i)
    d->probs[i] = calloc (l->classes + 1, sizeof (float));

  return d;
}


ssize_t
libdarknet_detect (libdarknet_detector_t d, float threshold,
                   float tree_threshold, const unsigned char *imgdata,
                   ssize_t datalen)
{
  const float nms = 0.4;        // WTF?
  const int channels = 3;
  int w, h, c;
  unsigned char *data =
    stbi_load_from_memory (imgdata, datalen, &w, &h, &c, channels);

  // XXX: from load_image_stb(char *filename, int channels) @ src/image.c
  c = channels;                 // WTF? Yes ^^
  image im = make_image (w, h, c);
  for (int k = 0; k < c; ++k)
    for (int j = 0; j < h; ++j)
      for (int i = 0; i < w; ++i)
        {
          int dst_index = i + w * j + w * h * k;
          int src_index = k + c * i + c * w * j;
          im.data[dst_index] = (float) data[src_index] / 255.0;
        }
  free (data);

  image sized = resize_image (im, d->net.w, d->net.h);
  free_image (im);

  layer *l = &d->net.layers[d->net.n - 1];
  (void) network_predict (d->net, sized.data);
  get_region_boxes (*l, 1, 1, threshold, d->probs, d->boxes, 0, NULL,
                    tree_threshold);

  int s = l->w * l->h * l->n;
  if (nms > 0.0)
    {
      if (l->softmax_tree)
        do_nms_obj (d->boxes, d->probs, s, l->classes, nms);
      else
        do_nms_sort (d->boxes, d->probs, s, l->classes, nms);
    }

  free_image (sized);

  d->nfound = 0;
  for (int i = 0; i < s; ++i)
    {
      int id = max_index (d->probs[i], l->classes);
      float prob = d->probs[i][id];
      if ((prob > threshold) && (d->nfound < d->nitems))
        {
          d->items[d->nfound].x = d->boxes[i].x;
          d->items[d->nfound].y = d->boxes[i].y;
          d->items[d->nfound].h = d->boxes[i].h;
          d->items[d->nfound].w = d->boxes[i].w;
          d->items[d->nfound].klass = id;
          d->items[d->nfound].confidence = prob;

          d->nfound++;
          if (d->nfound == d->nitems)
            {
              int ns = d->nitems * 2;
              libdarknet_item *tmp = realloc (d->items, ns * sizeof (libdarknet_item));
              if (NULL != tmp)
                {
                  d->items = tmp;
                  d->nitems = ns;
                }
            }
        }
    };

  return d->nfound;
}

const libdarknet_item *
libdarknet_get_items (libdarknet_detector_t d)
{
  return d->items;
}

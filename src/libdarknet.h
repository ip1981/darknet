#ifndef _LIBDARKNET_H
#define _LIBDARKNET_H

#include <stdlib.h>

typedef struct libdarknet_detector *libdarknet_detector_t;

typedef struct libdarknet_item
{
  int klass;                    /* Object class number.  */
  float confidence;             /* A.K.A. probability.  */
  float x;                      /* Horizontal coordinate of item's center.  */
  float y;                      /* Vertical coordinate of item's center.  */
  float h;                      /* Object's height.  */
  float w                       /* Object's width.  */
} libdarknet_item;

/*
 * How to use it:
 * 1. Create a new detector with libdarknet_new_detector("yolo.cfg", "yolo.weights").
 * 2. Get image data (e. g. open a file).
 * 3. Acquire a lock [in a multithreaded app].
 * 4. Call libdarknet_detect(...). It returns the number of detected objects.
 * 5. Call libdarknet_get_items(...). It returns a pointer to an array of the detected objects.
 * 6. Copy out that array (its length is returned by libdarknet_detect(...)).
 * 7. Release the lock [in a multithreaded app].
*/


libdarknet_detector_t libdarknet_new_detector (const char *
                                               /* network config file */ ,
                                               const char *
                                               /* network weights */ );

ssize_t libdarknet_detect (libdarknet_detector_t, float /* threshold */ ,
                           float /* tree threshold */ ,
                           const unsigned char * /* image data */ ,
                           ssize_t /* image data length */ );

const libdarknet_item *libdarknet_get_items (libdarknet_detector_t);

#endif

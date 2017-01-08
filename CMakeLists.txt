# Recommended configuration:
# CC=gcc-5 CXX=g++-5 LDFLAGS="-Wl,--no-undefined" CFLAGS="-Wall -Ofast" cmake ...


CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT (darknet)
INCLUDE (GNUInstallDirs)

OPTION (ENABLE_CUDA "Enable CUDA" ON)

SET (darknet_sources 
  src/activation_layer.c
  src/activation_layer.h
  src/activations.c
  src/activations.h
  src/avgpool_layer.c
  src/avgpool_layer.h
  src/batchnorm_layer.c
  src/batchnorm_layer.h
  src/blas.c
  src/blas.h
  src/box.c
  src/box.h
  src/col2im.c
  src/col2im.h
  src/connected_layer.c
  src/connected_layer.h
  src/convolutional_layer.c
  src/convolutional_layer.h
  src/cost_layer.c
  src/cost_layer.h
  src/crnn_layer.c
  src/crnn_layer.h
  src/crop_layer.c
  src/crop_layer.h
  src/cuda.c
  src/cuda.h
  src/data.c
  src/data.h
  src/deconvolutional_layer.c
  src/deconvolutional_layer.h
  src/detection_layer.c
  src/detection_layer.h
  src/dropout_layer.c
  src/dropout_layer.h
  src/gemm.c
  src/gemm.h
  src/gru_layer.c
  src/gru_layer.h
  src/im2col.c
  src/im2col.h
  src/image.c
  src/image.h
  src/layer.c
  src/layer.h
  src/list.c
  src/list.h
  src/local_layer.c
  src/local_layer.h
  src/matrix.c
  src/matrix.h
  src/maxpool_layer.c
  src/maxpool_layer.h
  src/network.c
  src/network.h
  src/normalization_layer.c
  src/normalization_layer.h
  src/option_list.c
  src/option_list.h
  src/parser.c
  src/parser.h
  src/region_layer.c
  src/region_layer.h
  src/reorg_layer.c
  src/reorg_layer.h
  src/rnn_layer.c
  src/rnn_layer.h
  src/route_layer.c
  src/route_layer.h
  src/shortcut_layer.c
  src/shortcut_layer.h
  src/softmax_layer.c
  src/softmax_layer.h
  src/stb_image.h
  src/tree.c
  src/tree.h
  src/utils.c
  src/utils.h
)

SET (darknet_cuda_sources
  src/activation_kernels.cu
  src/avgpool_layer_kernels.cu
  src/blas_kernels.cu
  src/col2im_kernels.cu
  src/convolutional_kernels.cu
  src/crop_layer_kernels.cu
  src/dropout_layer_kernels.cu
  src/im2col_kernels.cu
  src/maxpool_layer_kernels.cu
  src/network_kernels.cu
)

# These aren't part of original Darknet:
SET (libdarknet_sources
  src/libdarknet.c
  src/libdarknet.h
)


IF (ENABLE_CUDA)
  INCLUDE (FindCUDA)
  ADD_DEFINITIONS ( -DGPU )
  CUDA_BUILD_CLEAN_TARGET ()
  CUDA_ADD_LIBRARY (darknet SHARED ${darknet_sources} ${darknet_cuda_sources} ${libdarknet_sources})
  TARGET_LINK_LIBRARIES (darknet ${CUDA_curand_LIBRARY} ${CUDA_CUBLAS_LIBRARIES} ${CUDA_LIBRARIES})
ELSE (ENABLE_CUDA)
  SET (THREADS_PREFER_PTHREAD_FLAG ON)
  FIND_PACKAGE (Threads)
  ADD_LIBRARY (darknet SHARED ${darknet_sources} ${libdarknet_sources})
  TARGET_LINK_LIBRARIES (darknet m Threads::Threads)
ENDIF (ENABLE_CUDA)

INSTALL (TARGETS darknet
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL (FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/libdarknet.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)



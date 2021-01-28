set(BLUELAB_DEPS ${IPLUG2_DIR}/BL-Dependencies)

#
add_library(_darknet INTERFACE)
set(DARKNET_SRC "${BLUELAB_DEPS}/darknet/src/")
set(_darknet_src
  activation_layer.c
  activation_layer.h
  activations.c
  activations.h
  avgpool_layer.c
  avgpool_layer.h
  batchnorm_layer.c
  batchnorm_layer.h
  blas.c
  blas.h
  bl_defs.h
  bl_utils.c
  bl_utils.h
  box.c
  box.h
  boxmuller.c
  boxmuller.h
  classifier.h
  col2im.c
  col2im.h
  compare.c
  connected_layer.c
  connected_layer.h
  convolutional_layer2.c
  convolutional_layer2.h
  convolutional_layer.c
  convolutional_layer.h
  cost_layer.c
  cost_layer.h
  crnn_layer.c
  crnn_layer.h
  crop_layer.c
  crop_layer.h
  cuda.c
  cuda.h
  data.c
  data.h
  deconvolutional_layer2.c
  deconvolutional_layer2.h
  deconvolutional_layer.c
  deconvolutional_layer.h
  demo.c
  demo.h
  detection_layer.c
  detection_layer.h
  dk_stb_image.h
  dk_stb_image_write.h
  dk_utils.c
  dk_utils.h
  dropout_layer.c
  dropout_layer.h
  gemm-blas.c
  gemm.c
  gemm.h
  gru_layer.c
  gru_layer.h
  im2col.c
  im2col.h
  image.c
  image.h
  input_layer.c
  input_layer.h
  instancenorm_layer.c
  instancenorm_layer.h
  iseg_layer.c
  iseg_layer.h
  kernel_initializers.c
  kernel_initializers.h
  l2norm_layer.c
  l2norm_layer.h
  layer.c
  layer.h
  list.c
  list.h
  local_layer.c
  local_layer.h
  logistic_layer.c
  logistic_layer.h
  lstm_layer.c
  lstm_layer.h
  matrix.c
  matrix.h
  maxpool_layer.c
  maxpool_layer.h
  metrics_layer.c
  metrics_layer.h
  network.c
  network.h
  normalization_layer.c
  normalization_layer.h
  option_list.c
  option_list.h
  parser.c
  parser.h
  region_layer.c
  region_layer.h
  reorg_layer2.c
  reorg_layer2.h
  reorg_layer.c
  reorg_layer.h
  resample_layer.c
  resample_layer.h
  rnn_layer.c
  rnn_layer.h
  route_layer.c
  route_layer.h
  shortcut_layer.c
  shortcut_layer.h
  softmax_layer.c
  softmax_layer.h
  split_layer.c
  split_layer.h
  stb_image_resize.h
  tree.c
  tree.h
  upsample_layer.c
  upsample_layer.h
  yolo_layer.c
  yolo_layer.h
  )
list(TRANSFORM _darknet_src PREPEND "${DARKNET_SRC}")
iplug_target_add(_darknet INTERFACE
  INCLUDE ${BLUELAB_DEPS}/darknet/include
  ${BLUELAB_DEPS}/darknet/src
  SOURCE ${_darknet_src}
)

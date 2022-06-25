//
//  utest-col2im.c
//  bl-darknet
//
//  Created by applematuer on 9/19/20.
//  Copyright (c) 2020 applematuer. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "darknet.h"
#include "network.h"
#include "bl_utils.h"

#include "convolutional_layer2.h"
#include "convolutional_layer.h"
#include "deconvolutional_layer2.h"

#include "unit_tests.h"

#include "utest_conv_layer.h"

// 32, 256, 64
#define IMG_WIDTH 32
#define IMG_HEIGHT 32
#define IMG_CHAN 4

//3, 32, 31, 63, 5
#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3

// NOTE: crashes with NUM_FILTERS=1
#define NUM_FILTERS 4

// 1, 4
#define NUM_GROUPS 4

#define NUM_ITERS 10 // 100

#define BATCH_NORM 0

#define KERNEL_INITIALIZER ONES

#define CONV_DECONV_COPY_WEIGHTS 1

#define NUM_BATCHES 3 //5

// conv2
void
run_tests_conv_layer_nbatch_0(const char *test_name)
{
    int width = IMG_WIDTH;
    int height = IMG_HEIGHT;
    int nchan = IMG_CHAN;
    
    // image
    float *img00 = make_image0(width, height, nchan);
    draw_test_pattern(img00, width, height, nchan);
    
    // Duplicate image in batches
    int imgSize = width*height*nchan;
    float *img0 = calloc(1, imgSize*NUM_BATCHES*sizeof(float));
    for (int i = 0; i < NUM_BATCHES; i++)
    {
        memcpy(&img0[i*imgSize], img00, imgSize*sizeof(float));
    }
    
    char *fname0 = gen_fname(test_name, 0);
    bl_save_image_batch(width, height, nchan, NUM_BATCHES, img0, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    update_args update_args0;
    update_args0.batch = NUM_BATCHES;
    update_args0.learning_rate = 0.0000001; //0.00001;
    update_args0.momentum = 0.9;
    update_args0.decay = 0.0001;
    update_args0.adam = 0; //1;
    update_args0.B1 = 0.9;
    update_args0.B2 = 0.999;
    update_args0.eps = 1e-7;
    update_args0.t = 0;
    
    // conv2d
    int batch = NUM_BATCHES;
    int h = height;
    int w = width;
    int c = nchan;
    int n = NUM_FILTERS;
    int groups = NUM_GROUPS;
    int size_x = KERNEL_WIDTH; 
    int size_y = KERNEL_HEIGHT;
    int stride_x = 1;
    int stride_y = 1;
    int padding_x = size_x/2;
    int padding_y = size_y/2;
    ACTIVATION activation = LEAKY;
    int batch_normalize = BATCH_NORM;
    int binary = 0;
    int xnor = 0;
    int adam = 0;
    
    convolutional_layer2 l =
        make_convolutional_layer2(batch,
                                  h, w, c, n, groups,
                                  size_x, size_y,
                                  stride_x, stride_y,
                                  padding_x, padding_y,
                                  activation, batch_normalize,
                                  binary, xnor, adam,
                                  KERNEL_INITIALIZER, 0);
    l.learning_rate_scale = 1.0;
    
    network net;
    if (l.workspace_size > 0)
        net.workspace = calloc(1, l.workspace_size);
    
    net.input = img0;
    forward_convolutional_layer2(l, net);
    
    //
    int wchans = n;
    if (wchans > 4)
        wchans = 4;
    
    // weights
    char *fname1 = gen_fname(test_name, 1);
    bl_save_image(size_x, size_y, wchans/*n*/, l.weights, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    // output start
    int nchan_out = (l.out_c < 4) ? l.out_c : 4;
    
    char *fname2 = gen_fname(test_name, 2);
    //bl_save_image(l.out_w, l.out_h*NUM_BATCHES, nchan_out, l.output, fname2, FILL_IMG_ALPHA);
    bl_save_image_batch(l.out_w, l.out_h, nchan_out, NUM_BATCHES, l.output, fname2, FILL_IMG_ALPHA);
    free(fname2);
    
    net.input = img0;
    
    for (int i = 0; i < NUM_ITERS; i++)
    {
        forward_convolutional_layer2(l, net);

        int img_size = l.outputs*IMG_CHAN/NUM_FILTERS;
        compute_diff(l.output, img0/*net.input*/, l.delta,
                     img_size*NUM_BATCHES/*l.outputs*/);
        
        net.input = l.output;
        net.delta = l.delta;
        
        backward_convolutional_layer2(l, net);
        
        update_convolutional_layer2(l, update_args0);
        update_args0.t++;
    }
    
    // delta
    char *fname3 = gen_fname(test_name, 3);
    //bl_save_image(l.out_w, l.out_h*NUM_BATCHES, nchan_out, l.delta, fname3, FILL_IMG_ALPHA);
    bl_save_image_batch(l.out_w, l.out_h, nchan_out, NUM_BATCHES,
                        l.delta, fname3, FILL_IMG_ALPHA);
    free(fname3);
    
    // weights after iterations
    char *fname4 = gen_fname(test_name, 4);
    bl_save_image(size_x, size_y, wchans/*n*/, l.weights, fname4, FILL_IMG_ALPHA);
    free(fname4);
    
    // output after iterations
    net.input = img0;
    forward_convolutional_layer2(l, net);
    
    char *fname5 = gen_fname(test_name, 5);
    //bl_save_image(l.out_w, l.out_h*NUM_BATCHES, nchan_out, l.output, fname5, FILL_IMG_ALPHA);
    bl_save_image_batch(l.out_w, l.out_h, nchan_out, NUM_BATCHES,
                        l.output, fname5, FILL_IMG_ALPHA);
    free(fname5);
    
    //
    free(img0);
    free(img00);
}

// deconv2
void
run_tests_conv_layer_nbatch_1(const char *test_name)
{
    int width = IMG_WIDTH;
    int height = IMG_HEIGHT;
    int nchan = IMG_CHAN;
    
    // image
    float *img00 = make_image0(width, height, nchan);
    draw_test_pattern(img00, width, height, nchan);
    
    // Dumplicate image in batches
    int imgSize = width*height*nchan;
    float *img0 = calloc(1, imgSize*NUM_BATCHES*sizeof(float));
    for (int i = 0; i < NUM_BATCHES; i++)
    {
        memcpy(&img0[i*imgSize], img00, imgSize*sizeof(float));
    }

    char *fname0 = gen_fname(test_name, 0);
    //bl_save_image(width, height*NUM_BATCHES, nchan, img0, fname0, FILL_IMG_ALPHA);
    bl_save_image_batch(width, height, nchan, NUM_BATCHES,
                        img0, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    update_args update_args0;
    update_args0.batch = NUM_BATCHES;
    update_args0.learning_rate = 0.0000001; //0.00001;
    update_args0.momentum = 0.9;
    update_args0.decay = 0.0001;
    update_args0.adam = 0; //1;
    update_args0.B1 = 0.9;
    update_args0.B2 = 0.999;
    update_args0.eps = 1e-7;
    update_args0.t = 0;
    
    // deconv2d
    int batch = NUM_BATCHES;
    int h = height;
    int w = width;
    int c = nchan;
    int n = NUM_FILTERS;
    int groups = NUM_GROUPS;
    int size_x = KERNEL_WIDTH;
    int size_y = KERNEL_HEIGHT;
    int stride_x = 1;
    int stride_y = 1;
    int padding_x = size_x/2;
    int padding_y = size_y/2;
    ACTIVATION activation = LEAKY;
    int batch_normalize = BATCH_NORM;
    //int binary = 0;
    //int xnor = 0;
    int adam = 0;
    int out_padding_x = 0; //
    int out_padding_y = 0; //
    
    deconvolutional_layer2 l =
    make_deconvolutional_layer2(batch,
                              h, w, c, n, groups,
                              size_x, size_y,
                              stride_x, stride_y,
                              padding_x, padding_y,
                              out_padding_x, out_padding_y,
                              activation, batch_normalize,
                              //binary, xnor,
                              adam,
                              KERNEL_INITIALIZER, 0);
    
    l.learning_rate_scale = 1.0;

    network net;
    if (l.workspace_size > 0)
        net.workspace = calloc(1, l.workspace_size);
    
    net.input = img0;
    forward_deconvolutional_layer2(l, net);
    
    //
    int wchans = n;
    if (wchans > 4)
        wchans = 4;
    
    // weights
    char *fname1 = gen_fname(test_name, 1);
    bl_save_image(size_x, size_y, wchans/*n*/, l.weights, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    // output start
    int nchan_out = (l.out_c < 4) ? l.out_c : 4;
    
    char *fname2 = gen_fname(test_name, 2);
    //bl_save_image(l.out_w, l.out_h*NUM_BATCHES, nchan_out, l.output, fname2, FILL_IMG_ALPHA);
    bl_save_image_batch(l.out_w, l.out_h, nchan_out, NUM_BATCHES,
                        l.output, fname2, FILL_IMG_ALPHA);
    free(fname2);
    
    net.input = img0;
    
    for (int i = 0; i < NUM_ITERS; i++)
    {
        forward_deconvolutional_layer2(l, net);
        
        int img_size = l.outputs*IMG_CHAN/NUM_FILTERS;
        compute_diff(l.output, img0/*net.input*/, l.delta,
                     img_size*NUM_BATCHES/*l.outputs*/);
        
        net.input = l.output;
        net.delta = l.delta;
        
        backward_deconvolutional_layer2(l, net);
        
        update_deconvolutional_layer2(l, update_args0);
        update_args0.t++;
    }
    
    // delta
    char *fname3 = gen_fname(test_name, 3);
    //bl_save_image(l.out_w, l.out_h*NUM_BATCHES, nchan_out, l.delta, fname3, FILL_IMG_ALPHA);
    bl_save_image_batch(l.out_w, l.out_h, nchan_out, NUM_BATCHES,
                        l.delta, fname3, FILL_IMG_ALPHA);
    free(fname3);
    
    // weights after iterations
    char *fname4 = gen_fname(test_name, 4);
    bl_save_image(size_x, size_y, wchans/*n*/, l.weights, fname4, FILL_IMG_ALPHA);
    free(fname4);
    
    // output after iterations
    net.input = img0;
    forward_deconvolutional_layer2(l, net);
    
    char *fname5 = gen_fname(test_name, 5);
    //bl_save_image(l.out_w, l.out_h*NUM_BATCHES, nchan_out, l.output, fname5, FILL_IMG_ALPHA);
    bl_save_image_batch(l.out_w, l.out_h, nchan_out, NUM_BATCHES,
                        l.output, fname5, FILL_IMG_ALPHA);
    free(fname5);
    
    //
    free(img0);
    free(img00);
}

// conv2 then deconv2
void
run_tests_conv_layer_nbatch_2(const char *test_name)
{
    int width = IMG_WIDTH;
    int height = IMG_HEIGHT;
    int nchan = IMG_CHAN;
    
    // image
    float *img00 = make_image0(width, height, nchan);
    draw_test_pattern(img00, width, height, nchan);
    
    // Dumplicate image in batches
    int imgSize = width*height*nchan;
    float *img0 = calloc(1, imgSize*NUM_BATCHES*sizeof(float));
    for (int i = 0; i < NUM_BATCHES; i++)
    {
        memcpy(&img0[i*imgSize], img00, imgSize*sizeof(float));
    }
    
    char *fname0 = gen_fname(test_name, 0);
    //bl_save_image(width, height*NUM_BATCHES, nchan, img0, fname0, FILL_IMG_ALPHA);
    bl_save_image_batch(width, height, nchan, NUM_BATCHES,
                        img0, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    // conv2d
    int batch = NUM_BATCHES;
    int h = height;
    int w = width;
    int c = nchan;
    int n = NUM_FILTERS;
    int groups = NUM_GROUPS;
    int size_x = KERNEL_WIDTH;
    int size_y = KERNEL_HEIGHT;
    int stride_x = 1;
    int stride_y = 1;
    int padding_x = size_x/2;
    int padding_y = size_y/2;
    ACTIVATION activation = LEAKY;
    int batch_normalize = BATCH_NORM;
    int binary = 0;
    int xnor = 0;
    int adam = 0;
    
    int out_padding_x = 0; //
    int out_padding_y = 0; //
    
    // conv
    convolutional_layer2 l1 =
    make_convolutional_layer2(batch,
                              h, w, c, n, groups,
                              size_x, size_y,
                              stride_x, stride_y,
                              padding_x, padding_y,
                              activation, batch_normalize,
                              binary, xnor, adam,
                              KERNEL_INITIALIZER, 0);
    
    l1.learning_rate_scale = 1.0;

    // deconv
    deconvolutional_layer2 l2 =
    make_deconvolutional_layer2(batch,
                                h, w, c, n, groups,
                                size_x, size_y,
                                stride_x, stride_y,
                                padding_x, padding_y,
                                out_padding_x, out_padding_y,
                                activation, batch_normalize,
                                adam,
                                KERNEL_INITIALIZER, 0);
    
    l2.learning_rate_scale = 1.0;
    
#if CONV_DECONV_COPY_WEIGHTS
    // Copy weights
    for (int i = 0; i < l2.nweights; i++)
    {
        l2.weights[i] = l1.weights[i];
    }
#endif
    
    //
    int wchans = n;
    if (wchans > 4)
        wchans = 4;
    
    char *fname1 = gen_fname(test_name, 1);
    bl_save_image(size_x, size_y, wchans/*n*/, l1.weights, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    network net;

    size_t workspace_size = (l1.workspace_size > l2.workspace_size) ?
                                l1.workspace_size : l2.workspace_size;
    if (workspace_size > 0)
        net.workspace = calloc(1, workspace_size);
    
    //
    net.input = img0;
    forward_convolutional_layer2(l1, net);
    
    //
    int nchan_out1 = (l1.out_c < 4) ? l1.out_c : 4;
    
    char *fname2 = gen_fname(test_name, 2);
    //bl_save_image(l1.out_w, l1.out_h*NUM_BATCHES, nchan_out1, l1.output, fname2, FILL_IMG_ALPHA);
    bl_save_image_batch(l1.out_w, l1.out_h, nchan_out1, NUM_BATCHES,
                        l1.output, fname2, FILL_IMG_ALPHA);
    free(fname2);
    
    //
    net.input = l1.output;
    forward_deconvolutional_layer2(l2, net);
    
    int nchan_out2 = (l2.out_c < 4) ? l2.out_c : 4;
    
    char *fname3 = gen_fname(test_name, 3);
    //bl_save_image(l2.out_w, l2.out_h*NUM_BATCHES, nchan_out2, l2.output, fname3, FILL_IMG_ALPHA);
    bl_save_image_batch(l2.out_w, l2.out_h, nchan_out2, NUM_BATCHES,
                        l2.output, fname3, FILL_IMG_ALPHA);
    free(fname3);
    
    //
    free(img0);
    free(img00);
}

void
run_tests_conv_layer_nbatch()
{
    // See: https://petewarden.com/2015/04/20/why-gemm-is-at-the-heart-of-deep-learning/
    
    fprintf(stderr, "Running tests: conv_layer\n");
    
    run_tests_conv_layer_nbatch_0("conv_layer_nbatch_0");
    run_tests_conv_layer_nbatch_1("conv_layer_nbatch_1");
    run_tests_conv_layer_nbatch_2("conv_layer_nbatch_2");
}

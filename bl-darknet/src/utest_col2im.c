//
//  utest-col2im.c
//  bl-darknet
//
//  Created by applematuer on 9/19/20.
//  Copyright (c) 2020 applematuer. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "unit_tests.h"

#include "network.h"
#include "bl_utils.h"
#include "im2col.h"
#include "col2im.h"

#include "utest_col2im.h"

// NOTE: calling col2im() then im2col() is not reversible (and this is normal)
// See: https://www.programmersought.com/article/16963235/


//
void
run_tests_col2img_0(const char *test_name)
{
    int width = 128;
    int height = 128;
    int nchan = 4;
    
    float *img0 = make_image0(width, height, nchan);
    
    //
    draw_test_pattern(img0, width, height, nchan);
    
    char *fname = gen_fname(test_name, -1);
    bl_save_image(width, height, nchan, img0, fname, FILL_IMG_ALPHA);
    free(fname);
    
    free(img0);
}

void
run_tests_col2img_1(const char *test_name)
{
    int width = 64;
    int height = 128;
    int nchan = 4;
    
    float *img0 = make_image0(width, height, nchan);
    
    //
    draw_test_pattern(img0, width, height, nchan);
    
    char *fname = gen_fname(test_name, -1);
    bl_save_image(width, height, nchan, img0, fname, FILL_IMG_ALPHA);
    free(fname);
    
    free(img0);
}

void
run_tests_col2img_2(const char *test_name)
{
    int width = 128;
    int height = 64;
    int nchan = 4;
    
    float *img0 = make_image0(width, height, nchan);
    
    //
    draw_test_pattern(img0, width, height, nchan);
    
    char *fname = gen_fname(test_name, -1);
    bl_save_image(width, height, nchan, img0, fname, FILL_IMG_ALPHA);
    free(fname);
    
    free(img0);
}

void
run_tests_col2img_3(const char *test_name)
{
    int width = 128;
    int height = 128;
    int nchan = 4;
    
    int ksize_x = 3; //2;
    int ksize_y = 1;
    int stride_x = 1;
    int stride_y = 1;
    int pad_x = 1; //0;
    int pad_y = 0;
    
    int height_col = (height + 2*pad_y - ksize_y) / stride_y + 1;
    int width_col = (width + 2*pad_x - ksize_x) / stride_x + 1;
    int channels_col = nchan * ksize_x * ksize_y;
    
    float *img0 = make_image0(width, height, nchan);
    
    //
    draw_test_pattern(img0, width, height, nchan);
    
    char *fname0 = gen_fname(test_name, 0);
    bl_save_image(width, height, nchan, img0, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    //float *buf = alloc_image_buf(width, height, nchan);
    float *col = alloc_image_buf(width, height, channels_col);
    
    im2col_cpu2(img0,
                nchan,
                height, width,
                ksize_y, ksize_x,
                stride_y, stride_x,
                pad_y, pad_x,
                col);
    
    char *fname1 = gen_fname(test_name, 1);
    //bl_save_image(width*ksize_x, height*ksize_y, nchan, buf, fname1, FILL_IMG_ALPHA);
    bl_save_image(width_col, height_col, nchan, col, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    free(col);
    
    free(img0);
}

void
run_tests_col2img_4(const char *test_name)
{
    int width = 128;
    int height = 128;
    int nchan = 4;
    
    int ksize_x = 3; //1; //2;
    int ksize_y = 1;
    int stride_x = 1;
    int stride_y = 1;
    int pad_x = 1; //0;
    int pad_y = 0;
    
    int height_col = (height + 2*pad_y - ksize_y) / stride_y + 1;
    int width_col = (width + 2*pad_x - ksize_x) / stride_x + 1;
    int channels_col = nchan * ksize_x * ksize_y;
    
    float *col = make_image0(width_col, height_col, channels_col);
    
    //
    draw_test_pattern(col, width, height, nchan);
    
    char *fname0 = gen_fname(test_name, 0);
    bl_save_image(width_col, height_col, nchan, col, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    float *img = alloc_image_buf(width, height, nchan);
    
    col2im_cpu2(col,
                nchan, height, width,
                ksize_y, ksize_x,
                stride_y, stride_x,
                pad_y, pad_x,
                img);
    
    char *fname1 = gen_fname(test_name, 1);
    bl_save_image(width, height, nchan, img, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    free(img);
    
    free(col);
}

void
run_tests_col2img_5(const char *test_name)
{
    int width = 128;
    int height = 128;
    int nchan = 4;
    
    int ksize_x = 2;
    int ksize_y = 1;
    int stride_x = 1;
    int stride_y = 1;
    int pad_x = 0;
    int pad_y = 0;
    
    int height_col = (height + 2*pad_y - ksize_y) / stride_y + 1;
    int width_col = (width + 2*pad_x - ksize_x) / stride_x + 1;
    int channels_col = nchan * ksize_x * ksize_y;
    
    float *img0 = make_image0(width, height, nchan);
    
    //
    draw_test_pattern(img0, width, height, nchan);
    
    char *fname0 = gen_fname(test_name, 0);
    bl_save_image(width, height, nchan, img0, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    float *col = alloc_image_buf(width_col, height_col, channels_col);
    
    im2col_cpu2(img0,
                nchan,
                height, width,
                ksize_y, ksize_x,
                stride_y, stride_x,
                pad_y, pad_x,
                col);
    
    char *fname1 = gen_fname(test_name, 1);
    bl_save_image(width_col, height_col, nchan, col, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    col2im_cpu2(col,
                nchan,
                height, width,
                ksize_y, ksize_x,
                stride_y, stride_x,
                pad_y, pad_x,
                img0);
    
    char *fname2 = gen_fname(test_name, 2);
    bl_save_image(width, height, nchan, img0, fname2, FILL_IMG_ALPHA);
    free(fname2);
    
    free(col);
    free(img0);
}

// NOTE: calling col2im() then im2col() is not reversible (and this is normal)
// See: https://www.programmersought.com/article/16963235/
void
run_tests_col2img_6(const char *test_name)
{
    int width = 128;
    int height = 128;
    int nchan = 4;
    
    int ksize_x = 3; //1; // ORIG: 1
    int ksize_y = 1;
    int stride_x = 1;
    int stride_y = 1;
    int pad_x = 1; //0;
    int pad_y = 0;
    
    int height_col = (height + 2*pad_y - ksize_y) / stride_y + 1;
    int width_col = (width + 2*pad_x - ksize_x) / stride_x + 1;
    int channels_col = nchan * ksize_x * ksize_y;
    
    float *col = make_image0(width_col, height_col, channels_col);
    
    //
    draw_test_pattern(col, width, height, nchan);
    
    char *fname0 = gen_fname(test_name, 0);
    bl_save_image(width_col, height_col, nchan, col, fname0, FILL_IMG_ALPHA);
    free(fname0);
    
    float *img = alloc_image_buf(width, height, nchan);
    
    col2im_cpu2(col,
                nchan,
                height, width,
                ksize_y, ksize_x,
                stride_y, stride_x,
                pad_y, pad_x,
                img);
    
    char *fname1 = gen_fname(test_name, 1);
    bl_save_image(width, height, nchan, img, fname1, FILL_IMG_ALPHA);
    free(fname1);
    
    im2col_cpu2(img,
                nchan,
                height, width,
                ksize_y, ksize_x,
                stride_y, stride_x,
                pad_y, pad_x,
                col);
    
    char *fname2 = gen_fname(test_name, 2);
    bl_save_image(width_col, height_col, nchan, col, fname2, FILL_IMG_ALPHA);
    free(fname2);
    
    free(col);
    
    free(img);
}

void
run_tests_col2img()
{
    // See: https://petewarden.com/2015/04/20/why-gemm-is-at-the-heart-of-deep-learning/
    
    fprintf(stderr, "Running tests: col2img\n");
    
    run_tests_col2img_0("col2img_0");
    run_tests_col2img_1("col2img_1");
    run_tests_col2img_2("col2img_2");
    
    run_tests_col2img_3("col2img_3");
    run_tests_col2img_4("col2img_4");
    
    run_tests_col2img_5("col2img_5");
    run_tests_col2img_6("col2img_6");
}

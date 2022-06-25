//
//  unit-tests.c
//  bl-darknet
//
//  Created by applematuer on 9/16/20.
//  Copyright (c) 2020 applematuer. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "utest_col2im.h"
#include "utest_conv_layer.h"
#include "utest_rand_normal.h"
#include "utest_conv_layer_nbatch.h"

#include "unit_tests.h"

#define UNIT_TEST_DIR "unit-tests"

char *
gen_fname(const char *fname, int file_num)
{
    char *result = malloc(512*sizeof(char));
    
    if (file_num < 0)
        sprintf(result, "%s/%s", UNIT_TEST_DIR, fname);
    else
        sprintf(result, "%s/%s_%d", UNIT_TEST_DIR, fname, file_num);
    
    return result;
}

float *
make_image0(int width, int height, int nchan)
{
    float *img = malloc(width*height*nchan*sizeof(float));
    memset(img, 0, width*height*nchan*sizeof(float));
    
    // Fill alpha and next channels
    for (int k = 3; k < nchan; k++)
    {
        for (int i = 0; i < width*height; i++)
        {
            img[k*width*height + i] = 1.0;
        }
    }
    
    // Make a red and green ramp
    for (int j = 0; j < height; j++)
    {
        float t0 = ((float)j)/(height - 1);
        
        for (int i = 0; i < width; i++)
        {
            float t1 = ((float)i)/(width - 1);
            
            img[width*height*0 + (i + j*width)] = t0;
            img[width*height*1 + (i + j*width)] = t1;
            img[width*height*2 + (i + j*width)] = 0.5;            
        }
    }
    return img;
}

void
draw_test_pattern(float *img, int width, int height, int nchan)
{
    // Draw two rectangles, of different colors
#define NUM_RECTANGLES 2
    
    float rects[NUM_RECTANGLES][2][2] = { { { 0.2, 0.1 }, { 0.4, 0.4 } },
                                          { { 0.3, 0.2 }, { 0.5, 0.6 } } };
    
    int colors[NUM_RECTANGLES] = { 0.75, 0.9 };
    
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            for (int k = 0; k < NUM_RECTANGLES; k++)
            {
                if ((i >= rects[k][0][0]*width) && (j >= rects[k][0][1]*height) &&
                    (i <= rects[k][1][0]*width) && (j <= rects[k][1][1]*height))
                {
                    img[(i + j*width) + k*width*height] = colors[k];
                }
            }
        }
    }
}

float *
alloc_image_buf(int width, int height, int nchan)
{
    float *img_buf = malloc(width*height*nchan*sizeof(float));
    memset(img_buf, 0, width*height*nchan*sizeof(float));
    
    return img_buf;
}

void
compute_diff(const float *a, const float *b, float *c, int size)
{
    for (int i = 0; i < size; i++)
        c[i] = a[i] - b[i];
}

//
void
run_unit_tests(int argc, char **argv)
{
    fprintf(stderr, "Running unit tests...\n");
    
    run_tests_col2img();
    //run_tests_conv_layer();
    //run_tests_rand_normal();
    //run_tests_conv_layer_nbatch();
    
    fprintf(stderr, "Done!\n");
}

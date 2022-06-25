//
//  utest-rand-normal.c
//  bl-darknet
//
//  Created by applematuer on 9/20/20.
//  Copyright (c) 2020 applematuer. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "dk_utils.h"

#include "unit_tests.h"
#include "bl_utils.h"

#include "utest_rand_normal.h"

#define BUF_SIZE 1024

void
run_tests_rand_normal_0(const char *test_name)
{
    float buf[BUF_SIZE];
    for (int i = 0; i < BUF_SIZE; i++)
    {
        float r = ((float)rand())/RAND_MAX;
        
        r = r*2.0 - 1.0;
        
        buf[i] = r;
    }
    
    char *fname = gen_fname(test_name, -1);
    bl_save_data_txt(BUF_SIZE, buf, fname);
    free(fname);
}

void
run_tests_rand_normal_1(const char *test_name)
{
    float mean = 0.0;
    float stdev = 1.0;
    
    float buf[BUF_SIZE];
    for (int i = 0; i < BUF_SIZE; i++)
    {
        float r = rand_normal();
        buf[i] = r;
    }
    
    char *fname = gen_fname(test_name, -1);
    bl_save_data_txt(BUF_SIZE, buf, fname);
    free(fname);
}

void
run_tests_rand_normal_2(const char *test_name)
{
    float mean = 0.0;
    float stdev = 1.0;
    
    float buf[BUF_SIZE];
    for (int i = 0; i < BUF_SIZE; i++)
    {
        float r = bl_rand_normal(mean, stdev);
        buf[i] = r;
    }
    
    char *fname = gen_fname(test_name, -1);
    bl_save_data_txt(BUF_SIZE, buf, fname);
    free(fname);
}

void
run_tests_rand_normal()
{
    fprintf(stderr, "Running tests: rand_normal\n");
    
    run_tests_rand_normal_0("rand_normal_0");
    run_tests_rand_normal_1("rand_normal_1");
    run_tests_rand_normal_2("rand_normal_2");
}


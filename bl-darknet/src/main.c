//
//  main.c
//  bl-darknet
//
//  Created by applematuer on 6/7/20.
//  Copyright (c) 2020 applematuer. All rights reserved.
//

#include <stdio.h>

#ifdef __APPLE__
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#else // Linux
#include <stb_image.h>
#include <stb_image_write.h>
#endif

#include <darknet.h>

// contained in darknet
#include <denormal.h>

#include "unit_tests.h"

extern void run_rebalance(int argc, char **argv);

int
main(int argc, const char * argv[])
{
    if(argc < 2){
        fprintf(stderr, "usage: %s <function>\n", argv[0]);
        return 0;
    }
    gpu_index = find_int_arg(argc, (char **)argv, "-i", 0);
    if(find_arg(argc, (char **)argv, "-nogpu")) {
        gpu_index = -1;
    }
    
    char *gpu_list = find_char_arg(argc, (char **)argv, "-gpus", "");
    int ngpus;
    int *gpus = read_intlist(gpu_list, &ngpus, gpu_index);
    
    if (ngpus == 1 || gpu_index < 0) {
        gpus[0] = gpu_index;
    }

    denormal_flushtozero();
        
#ifndef GPU
    gpu_index = -1;
#else
    //if (gpu_index >= 0) {
    //    gpusg = gpus;
    //    ngpusg = ngpus;
    //    gpu_index = gpus[0];
    //    opencl_init(gpus, ngpus);
    //}
    if(gpu_index >= 0){
        cuda_set_device(gpu_index);
#endif
    
    if (0 == strcmp(argv[1], "rebalance"))
    {
        run_rebalance(argc, (char **)argv);
    }
    else if (0 == strcmp(argv[1], "unit-tests"))
    {
        run_unit_tests(argc, (char **)argv);
    }
    else
    {
        fprintf(stderr, "Not an option: %s\n", argv[1]);
    }
    
#ifdef GPU
    //if (gpu_index >= 0) {
    //    opencl_deinit(gpus, ngpus);
    //}
    //free(gpus);
#endif

    //denormal_reset();
    
    return 0;
}


//
//  rebalance.c
//  bl-darknet
//
//  Created by applematuer on 6/7/20.
//  Copyright (c) 2020 applematuer. All rights reserved.
//

#include <math.h>

#include <unistd.h>

#include <darknet.h>

#include "dk_utils.h"
#include "bl_utils.h"

#define EPS 1e-15

//#define IMAGE_WIDTH  1024
#define IMAGE_WIDTH  256
#define IMAGE_HEIGHT 32
#define IMAGE_CHANS  4

// Train
#define TRAIN_RATIO 0.5 //0.8

#define TRAIN_DIR "training/"
#define DUMP_DATASET_DIR "dataset/"

// Sleep every 5mn
#define TRAIN_SLEEP_EVERY 300
// Sleep 30 seconds
#define TRAIN_SLEEP_TIME 60 //30

//#define NUM_DATA_EPOCH_RATIO 1.0
#define NUM_DATA_EPOCH_RATIO 0.1
//#define NUM_DATA_EPOCH_RATIO 0.01

//#define NUM_DATA_TEST_RATIO 0.2
//#define NUM_DATA_TEST_RATIO 0.01
//#define NUM_DATA_TEST_RATIO 0.05
#define NUM_DATA_TEST_RATIO NUM_DATA_EPOCH_RATIO*0.1

// NOTE: must keep it to 1, even if we change [metrics] in model .cfg
#define METRICS_MSE_MASK 0 //1 // ORIG
#define METRICS_L1_MASK 1 // TEST

//
// WIP TEST
#define DEBUG_TRAIN_NET 0 //1 //0
#define DEBUG_TEST_NET 1 //0 // 1

#define DEBUG_DUMP_DATA 1
#define DEBUG_DUMP_WEIGHTS 1
#define DEBUG_DUMP_TXT 1 //0 //1

// Force using a single sample each time, for debugging
#define DEBUG_FORCE_SAMPLE_NUM -1
//#define DEBUG_FORCE_SAMPLE_NUM 501
#define DEBUG_FORCE_SAMPLE_NUM_SAMPLES 1 //4

//#define DEBUG_FORCE_SAMPLE_NUM 503 // For 1 song
//#define DEBUG_FORCE_SAMPLE_NUM_TEST 503 // for 1st mel

// ORIG: good
//#define DEBUG_FORCE_SAMPLE_NUM_TEST 501 // For mel2
#define DEBUG_FORCE_SAMPLE_NUM_TEST 2005 // For mel3 + overlap
// TEST stereo width
//#define DEBUG_FORCE_SAMPLE_NUM_TEST 200

// Does not normalize "truth" between 0 and 1 (the max values goes to around 100)
#define NORMALIZE_INPUTS 0 //1 //0 //1
// Normalizes correctly "truth" between 0 and 1.
#define NORMALIZE_INPUTS2 1 //0 //1 //0
#define NORMALIZE_INPUTS3 0 // 1 // TEST 2 (fail)

// Test: compute db from amp magns
// NOTE: bad!
#define INPUTS_TO_DB 1 //0

// Instance num
// See: https://www.researchgate.net/publication/320966887_Image-to-Image_Translation_with_Conditional_Adversarial_Networks#pfe
//#define USE_INSTANCE_NORM_METHOD 1
//#define INST_NORM_NUM_TEST_DATA 100

//
#define MODE_TRAIN 0
#define MODE_TEST  1

// Avoid that train score gets negative
#define FIX_NEGATIVE_METRICS 1

#define DATA_AUGMENTATION 0 //1
#define DATA_AUGMENTATION_RATIO_X 0.1
#define DATA_AUGMENTATION_RATIO_Y 0.1

// Dump metrics data when predicting
#define DEBUG_METRICS 1

#define USE_STEREO_WIDTH 0 //1
#define FIX_INPUT_DATA 1

// Binary
#define USE_BINARY_MASKS 1 //0 //1

#if USE_BINARY_MASKS
#define METRICS_L1_MASK 0
#endif

// Was 1 for training 2 months
#define X_IS_STEM_SUM 0 //1

#if 0
For im2col and other useful explanations about convolution:
https://petewarden.com/2015/04/20/why-gemm-is-at-the-heart-of-deep-learning/

NOTE: this looks normal to have zeros between dots when striding,
see: https://github.com/vdumoulin/conv_arithmetic
and: https://datascience.stackexchange.com/questions/6107/what-are-deconvolutional-layers
#endif

// Shuffle maps
long *_full_shuffle_map = NULL;
long *_train_shuffle_map = NULL;
long *_test_shuffle_map = NULL;

typedef struct
{
    float *data;
    long n;
} images;

void
generate_shuffle_map(long int **shuffle_map, long int n)
{
    if (*shuffle_map != NULL)
        free(*shuffle_map);
    
    *shuffle_map = calloc(sizeof(long int), n);
    
    long int i;
    for(i = 0; i < n; i++)
        (*shuffle_map)[i] = i;
    
    for(i = 0; i < n; i++)
    {
        long int index = rand() % n;
        
        long int tmp = (*shuffle_map)[i];
        (*shuffle_map)[i] = (*shuffle_map)[index];
        (*shuffle_map)[index] = tmp;
    }
}

void
amp_to_db(float *buf, int size)
{
    for (int i = 0; i < size; i++)
    {
        float v = buf[i];
        v = bl_amp_to_db_norm(v);
        buf[i] = v;
    }
}

// For debugging
void
load_txt_data(const char *filename, float *data, long int n)
{
    FILE *file = fopen(filename, "rb");
    
    long int i;
    for (i = 0; i < n; i++)
        fscanf(file, "%f", &data[i]);
    
    fclose(file);
}

void
free_images(images imgs)
{
    free(imgs.data);
}

long int
get_num_data_samples(const char *filename)
{
    //if (DEBUG_FORCE_SAMPLE_NUM >= 0)
    //    return DEBUG_FORCE_SAMPLE_NUM_SAMPLES;

    // DEBUG
    //return 32;
    
    long int sz = bl_file_size(filename);
    
    long int num = sz/(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    return num;
}

void
swap_channels(float *img_data, int chan_size, int num_chans)
{
#define CHAN0 0 // red
//#define CHAN1 2 // blue
#define CHAN1 3 // cyan
    float *tmp = malloc(chan_size*sizeof(float));
    
    // Copy 1st channel to tmp
    memcpy(tmp, &img_data[chan_size*CHAN0], chan_size*sizeof(float));
    
    // Copy 2nd channel to 1st channel
    memcpy(&img_data[chan_size*CHAN0], &img_data[chan_size*CHAN1], chan_size*sizeof(float));
    
    // Copy tmp to 2nd channel
    memcpy(&img_data[chan_size*CHAN1], tmp, chan_size*sizeof(float));
    
    free(tmp);
}

#if 0 // should be removed later
data
random_data_sample(const char *mixfile,
                   const char *stemfile0, const char *stemfile1,
                   const char *stemfile2, const char *stemfile3,
                   int mode, long int index)
{
    // Get sample index
    long int total_num_data = get_num_data_samples(mixfile);
    
    long int start_data = 0;
    if (mode == MODE_TEST)
        start_data = total_num_data*TRAIN_RATIO;
    
    //
    long int index0;
    if (mode == MODE_TRAIN)
        index0 = _train_shuffle_map[index];
    else
        index0 = _test_shuffle_map[index];
        
    index0 += start_data;
    index0 = _full_shuffle_map[index0];
    
    if (DEBUG_FORCE_SAMPLE_NUM >= 0)
        index0 = DEBUG_FORCE_SAMPLE_NUM;
    
    // Result
    data d = {0};
    d.X = make_matrix(1, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    d.y = make_matrix(1, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    FILE *mixfp = fopen(mixfile, "rb");
    
    FILE *stemfp0 = fopen(stemfile0, "rb");
    FILE *stemfp1 = fopen(stemfile1, "rb");
    FILE *stemfp2 = fopen(stemfile2, "rb");
    FILE *stemfp3 = fopen(stemfile3, "rb");
    
    // Set file position
    long int file_pos = index0*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float);
    fseek(mixfp, file_pos, SEEK_SET);

    fseek(stemfp0, file_pos, SEEK_SET);
    fseek(stemfp1, file_pos, SEEK_SET);
    fseek(stemfp2, file_pos, SEEK_SET);
    fseek(stemfp3, file_pos, SEEK_SET);
    
    // Read
    // OLD
    //fread(d.X.vals[0], sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, mixfp);
    //memcpy(&d.X.vals[0][1*IMAGE_WIDTH*IMAGE_HEIGHT],
    //       d.X.vals[0],
    //       IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    //memcpy(&d.X.vals[0][2*IMAGE_WIDTH*IMAGE_HEIGHT],
    //       d.X.vals[0],
    //       IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    //memcpy(&d.X.vals[0][3*IMAGE_WIDTH*IMAGE_HEIGHT],
    //       d.X.vals[0],
    //       IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));

    fread(&d.y.vals[0][0*IMAGE_WIDTH*IMAGE_HEIGHT],
          sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp0);
    fread(&d.y.vals[0][1*IMAGE_WIDTH*IMAGE_HEIGHT],
          sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp1);
    fread(&d.y.vals[0][2*IMAGE_WIDTH*IMAGE_HEIGHT],
          sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp2);
    fread(&d.y.vals[0][3*IMAGE_WIDTH*IMAGE_HEIGHT],
          sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp3);
    
//#if INPUTS_TO_DB
//    //amp_to_db(d.X.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
//    amp_to_db(d.y.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
//#endif
    
    // NEW
    for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT; i++)
    {
        d.X.vals[0][i] = d.y.vals[0][i + 0*IMAGE_WIDTH*IMAGE_HEIGHT];
        d.X.vals[0][i] += d.y.vals[0][i + 1*IMAGE_WIDTH*IMAGE_HEIGHT];
        d.X.vals[0][i] += d.y.vals[0][i + 2*IMAGE_WIDTH*IMAGE_HEIGHT];
        d.X.vals[0][i] += d.y.vals[0][i + 3*IMAGE_WIDTH*IMAGE_HEIGHT];
        
        d.X.vals[0][i + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[0][i];
        d.X.vals[0][i + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[0][i];
        d.X.vals[0][i + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[0][i];
    }
  
#if INPUTS_TO_DB
    amp_to_db(d.X.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    amp_to_db(d.y.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
    
#if NORMALIZE_INPUTS
    bl_normalize2(d.X.vals[0], d.y.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS); // origin
    // TODO: normalize by brightness ?
#endif
#if NORMALIZE_INPUTS2
    bl_normalize(d.X.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    bl_normalize(d.y.vals[0], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
#if NORMALIZE_INPUTS3
    bl_normalize_chan(d.X.vals[0], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
    bl_normalize_chan(d.y.vals[0], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
    
    fclose(mixfp);
    
    fclose(stemfp0);
    fclose(stemfp1);
    fclose(stemfp2);
    fclose(stemfp3);
    
    return d;
}
#endif

void
augment_data(matrix X, matrix y, int index)
{
#if 0 // Debug
    image image0 = make_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS);
    memcpy(image0.data, X.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                  image0.data, "dbg/x0", 1);
    free_image(image0);
#endif
    
#if 0 // Debug
    image image0 = make_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS);
    memcpy(image0.data, y.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                  image0.data, "dbg/y0", IMAGE_CHANS);
    free_image(image0);
#endif
    
    // Copy the stems to separate images
    image stem_imgs[IMAGE_CHANS];
    for (int k = 0; k < IMAGE_CHANS; k++)
    {
        stem_imgs[k] = make_image(IMAGE_WIDTH, IMAGE_HEIGHT, 1);
        memcpy(stem_imgs[k].data, &y.vals[index][k*IMAGE_WIDTH*IMAGE_HEIGHT],
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    }
    
    // Scale and offset the stems, to augment data
    //
    for (int k = 0; k < IMAGE_CHANS; k++)
    {
        // Scale
        float scale_x = 1.0f + ((double)rand()/RAND_MAX)*DATA_AUGMENTATION_RATIO_X;
        float scale_y = 1.0f + ((double)rand()/RAND_MAX)*DATA_AUGMENTATION_RATIO_Y;
        
        int scaled_width = scale_x*IMAGE_WIDTH;
        int scaled_height = scale_y*IMAGE_HEIGHT;
        
        image scaled_img = resize_image(stem_imgs[k], scaled_width, scaled_height);
        
        // Crop
        int crop_dx = 0;
        //if (scaled_width - IMAGE_WIDTH > 0)
        //    crop_dx = rand() % (scaled_width - IMAGE_WIDTH);
        crop_dx = 0; // Stick the frequencies at the bottom
        int crop_dy = 0;
        if (scaled_height - IMAGE_HEIGHT > 0)
            crop_dy = rand() % (scaled_height - IMAGE_HEIGHT);
        
        image croped_image = crop_image(scaled_img,
                                        crop_dx, crop_dy,
                                        IMAGE_WIDTH, IMAGE_HEIGHT);
        
        
        memcpy(stem_imgs[k].data, croped_image.data,
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        
        free_image(croped_image);
        free_image(scaled_img);
    }
    
    // Copy back the images to y
    for (int k = 0; k < IMAGE_CHANS; k++)
    {
        memcpy(&y.vals[index][k*IMAGE_WIDTH*IMAGE_HEIGHT], stem_imgs[k].data,
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    }
    
    // Sum the stems to get the mix, X
    //
    
    // Add the stems to get the mix
    // (warning: the addition is not the same at 100% compared to the resla mix)
    memset(X.vals[index], 0, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    
    // Fill the first channel with the sum
    for (int j = 0; j < IMAGE_WIDTH*IMAGE_HEIGHT; j++)
    {
        //for (int k = 0; k < IMAGE_CHANS; k++)
        //    X.vals[index][j] += y.vals[index][j + k*IMAGE_WIDTH*IMAGE_HEIGHT];
        
        for (int k = 0; k < IMAGE_CHANS; k++)
            X.vals[index][j] += stem_imgs[k].data[j];
    }
    
    // Duplicate the first channel
    memcpy(&X.vals[index][1*IMAGE_WIDTH*IMAGE_HEIGHT],
           X.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    memcpy(&X.vals[index][2*IMAGE_WIDTH*IMAGE_HEIGHT],
           X.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    memcpy(&X.vals[index][3*IMAGE_WIDTH*IMAGE_HEIGHT],
           X.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    // Free
    for (int k = 0; k < IMAGE_CHANS; k++)
    {
        free_image(stem_imgs[k]);
    }
    
#if 0 // Debug
    image image1 = make_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS);
    memcpy(image1.data, X.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                  image1.data, "dbg/x1", 1);
    free_image(image1);
#endif
    
#if 0 // Debug
    image image1 = make_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS);
    memcpy(image1.data, y.vals[index], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                  image1.data, "dbg/y1", IMAGE_CHANS);
    free_image(image1);
#endif
}

data
random_data_samples(const char *mixfile0, const char *mixfile1,
                    const char *stemfile0, const char *stemfile1,
                    const char *stemfile2, const char *stemfile3,
                    int mode,
                    long int start_index, long int num_data,
                    int data_augmentation)// ,
                    //int *source_silence)
{
    // Result
    data d = {0};
    d.X = make_matrix((int)num_data, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    d.y = make_matrix((int)num_data, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    // Get sample index
    long int total_num_data = get_num_data_samples(mixfile0);
    
    long int start_data = 0;
    if (mode == MODE_TEST)
        start_data = total_num_data*TRAIN_RATIO;
    
    FILE *mixfp0 = fopen(mixfile0, "rb");
    FILE *mixfp1 = fopen(mixfile1, "rb");
    
    FILE *stemfp0 = fopen(stemfile0, "rb");
    FILE *stemfp1 = fopen(stemfile1, "rb");
    FILE *stemfp2 = fopen(stemfile2, "rb");
    FILE *stemfp3 = fopen(stemfile3, "rb");
    
    for (int i = 0; i < num_data; i++)
    {
        //
        long int index0;
        if (mode == MODE_TRAIN)
            index0 = _train_shuffle_map[start_index + i];
        else
            index0 = _test_shuffle_map[start_index + i];
    
        index0 += start_data;
        index0 = _full_shuffle_map[index0];
    
        if (DEBUG_FORCE_SAMPLE_NUM >= 0)
        {
            index0 = DEBUG_FORCE_SAMPLE_NUM;
            
            // Choose between the next samples
            index0 += rand() % DEBUG_FORCE_SAMPLE_NUM_SAMPLES;
        }
        
        // Set file position
        long int file_pos = index0*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float);
        
        fseek(mixfp0, file_pos, SEEK_SET);
        fseek(mixfp1, file_pos, SEEK_SET);
        
        fseek(stemfp0, file_pos, SEEK_SET);
        fseek(stemfp1, file_pos, SEEK_SET);
        fseek(stemfp2, file_pos, SEEK_SET);
        fseek(stemfp3, file_pos, SEEK_SET);
    
        // Read the mix from the mix file
        if (!data_augmentation)
        {
            // OLD
            //fread(d.X.vals[i], sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, mixfp);
        
            //memcpy(&d.X.vals[i][1*IMAGE_WIDTH*IMAGE_HEIGHT],
            //       d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        
            //memcpy(&d.X.vals[i][2*IMAGE_WIDTH*IMAGE_HEIGHT],
            //       d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        
            //memcpy(&d.X.vals[i][3*IMAGE_WIDTH*IMAGE_HEIGHT],
            //       d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        }
        
        // DEBUG
        //swap_channels(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT, 4);
        
        fread(&d.y.vals[i][0*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp0);
        fread(&d.y.vals[i][1*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp1);
        fread(&d.y.vals[i][2*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp2);
        fread(&d.y.vals[i][3*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp3);
    
        // NEW
//#if INPUTS_TO_DB
//        amp_to_db(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
//#endif

        // NEW
        for (int j = 0; j < IMAGE_WIDTH*IMAGE_HEIGHT; j++)
        {
            d.X.vals[i][j] = d.y.vals[i][j + 0*IMAGE_WIDTH*IMAGE_HEIGHT];
            d.X.vals[i][j] += d.y.vals[i][j + 1*IMAGE_WIDTH*IMAGE_HEIGHT];
            d.X.vals[i][j] += d.y.vals[i][j + 2*IMAGE_WIDTH*IMAGE_HEIGHT];
            d.X.vals[i][j] += d.y.vals[i][j + 3*IMAGE_WIDTH*IMAGE_HEIGHT];
            
            d.X.vals[i][j + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
        }
        
        // was DEBUG
        //swap_channels(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT, 4);
        
        /*
        if (source_silence != NULL)
        {
            // Check if a source is totally silent
            *source_silence = 0;
            for (int k = 0; k < 4; k++)
            {
                int src_silent = 1;
                for (int j = 0; j < IMAGE_WIDTH*IMAGE_HEIGHT; j++)
                {
                    float val = d.y.vals[i][j + k*IMAGE_WIDTH*IMAGE_HEIGHT];
                    if (val > 1e-15)
                    {
                        src_silent = 0;
                        break;
                    }
                }
            
                if (src_silent)
                {
                    *source_silence = 1;
                    break;
                }
            }
        }
        */
      
        // Sum the stems to get X
        // (instead of loading it)
        if (data_augmentation)
        {
            augment_data(d.X, d.y, i);
        }
        
#if INPUTS_TO_DB
        amp_to_db(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        amp_to_db(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        
#if NORMALIZE_INPUTS
        bl_normalize2(d.X.vals[i], d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        // TODO: normalize by brightness ?
#endif
#if NORMALIZE_INPUTS2
        bl_normalize(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        
#if !USE_BINARY_MASKS
        bl_normalize(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
#endif
        
#if NORMALIZE_INPUTS3
        bl_normalize_chan(d.X.vals[0], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
        bl_normalize_chan(d.y.vals[0], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
    }
    
    fclose(mixfp0);
    fclose(mixfp1);
    
    fclose(stemfp0);
    fclose(stemfp1);
    fclose(stemfp2);
    fclose(stemfp3);
    
    return d;
}

data
random_data_samples2(const char *mixfile0, const char *mixfile1,
                     const char *stemfile0, const char *stemfile1,
                     const char *stemfile2, const char *stemfile3,
                     int mode,
                     long int start_index, long int num_data,
                     int data_augmentation,
                     float **dataNoSt)// ,
//int *source_silence)
{
    // Result
    data d = {0};
    d.X = make_matrix((int)num_data, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    d.y = make_matrix((int)num_data, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    if (dataNoSt != NULL)
        *dataNoSt = calloc((int)num_data*IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS, sizeof(float));
    
    // Get sample index
    long int total_num_data = get_num_data_samples(mixfile0);
    
    long int start_data = 0;
    if (mode == MODE_TEST)
        start_data = total_num_data*TRAIN_RATIO;
    
    FILE *mixfp0 = fopen(mixfile0, "rb");
    
#if USE_STEREO_WIDTH
    FILE *mixfp1 = fopen(mixfile1, "rb");
#endif
    
    FILE *stemfp0 = fopen(stemfile0, "rb");
    FILE *stemfp1 = fopen(stemfile1, "rb");
    FILE *stemfp2 = fopen(stemfile2, "rb");
    FILE *stemfp3 = fopen(stemfile3, "rb");
    
    for (int i = 0; i < num_data; i++)
    {
        //
        long int index0;
        if (mode == MODE_TRAIN)
            index0 = _train_shuffle_map[start_index + i];
        else
            index0 = _test_shuffle_map[start_index + i];
        
        index0 += start_data;
        index0 = _full_shuffle_map[index0];
        
        if (DEBUG_FORCE_SAMPLE_NUM >= 0)
        {
            index0 = DEBUG_FORCE_SAMPLE_NUM;
            
            // Choose between the next samples
            index0 += rand() % DEBUG_FORCE_SAMPLE_NUM_SAMPLES;
        }
        
        // Set file position
        long int file_pos = index0*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float);
        
        fseek(mixfp0, file_pos, SEEK_SET);
        
#if USE_STEREO_WIDTH
        fseek(mixfp1, file_pos, SEEK_SET);
#endif
        
        fseek(stemfp0, file_pos, SEEK_SET);
        fseek(stemfp1, file_pos, SEEK_SET);
        fseek(stemfp2, file_pos, SEEK_SET);
        fseek(stemfp3, file_pos, SEEK_SET);
        
        // DEBUG
        //swap_channels(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT, 4);
        
        fread(&d.y.vals[i][0*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp0);
        fread(&d.y.vals[i][1*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp1);
        fread(&d.y.vals[i][2*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp2);
        fread(&d.y.vals[i][3*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp3);

#if X_IS_STEM_SUM // Prev, training 2 months
        for (int j = 0; j < IMAGE_WIDTH*IMAGE_HEIGHT; j++)
        {
            d.X.vals[i][j] = d.y.vals[i][j + 0*IMAGE_WIDTH*IMAGE_HEIGHT];
            d.X.vals[i][j] += d.y.vals[i][j + 1*IMAGE_WIDTH*IMAGE_HEIGHT];
            d.X.vals[i][j] += d.y.vals[i][j + 2*IMAGE_WIDTH*IMAGE_HEIGHT];
            d.X.vals[i][j] += d.y.vals[i][j + 3*IMAGE_WIDTH*IMAGE_HEIGHT];
            
            d.X.vals[i][j + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
        }
#else
        fread(&d.X.vals[i][0],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, mixfp0);
        
        for (int j = 0; j < IMAGE_WIDTH*IMAGE_HEIGHT; j++)
        {
            d.X.vals[i][j + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
        }
#endif
        
        // was DEBUG
        //swap_channels(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT, 4);
        
        // Sum the stems to get X
        // (instead of loading it)
        if (data_augmentation)
        {
            augment_data(d.X, d.y, i);
        }
        
#if INPUTS_TO_DB
        amp_to_db(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        amp_to_db(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        
#if !USE_BINARY_MASKS
#if NORMALIZE_INPUTS
        bl_normalize2(d.X.vals[i], d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        // TODO: normalize by brightness ?
#endif
#if NORMALIZE_INPUTS2
        bl_normalize(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        bl_normalize(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        
#if NORMALIZE_INPUTS3
        bl_normalize_chan(d.X.vals[0], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
        bl_normalize_chan(d.y.vals[0], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
#endif
      
#if USE_BINARY_MASKS
        bl_binarize(d.y.vals[i], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
        
#if USE_STEREO_WIDTH
        if (dataNoSt != NULL)
            memcpy(&((*dataNoSt)[i*IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS]),
                   d.X.vals[i],
                   IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
        
        fread(&d.X.vals[i][1*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, mixfp1);
        // Normalize stereo width
        bl_normalize(&d.X.vals[i][1*IMAGE_WIDTH*IMAGE_HEIGHT], IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
    }
    
    fclose(mixfp0);
    
#if USE_STEREO_WIDTH
    fclose(mixfp1);
#endif
    
    fclose(stemfp0);
    fclose(stemfp1);
    fclose(stemfp2);
    fclose(stemfp3);
    
    return d;
}
images
load_one_image(const char *filename, long int index)
{
    images img;
    img.n = 1;
    img.data = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT);
    
    long int num_data = get_num_data_samples(filename);
    
    if (index > num_data)
        // Return empty image
        return img;
    
    //
    long int file_pos = index*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float);
    
    FILE *fp = fopen(filename, "rb");
    
    fseek(fp, file_pos, SEEK_SET);
    fread(img.data, sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, fp);
    
    fclose(fp);
    
//#if INPUTS_TO_DB
//    amp_to_db(img.data, IMAGE_WIDTH*IMAGE_HEIGHT);
//#endif
    
    return img;
}

float
compute_metrics_l1mask_norm(float *X, float *y, float *pred, long int size)
{
    // [metrics], L1mask_norm
    
    mult_arrays(pred, X, (int)size);
    
    long int i;
    float *output = calloc(sizeof(float), size);
    
    for(i = 0; i < size; ++i)
    {
        float diff = y[i] - pred[i];
        float abs_val = fabs(diff);
        output[i] = abs_val;
    }
    
    float mean_data = mean_array(X, (int)size);
    if (mean_data > 0.0)
    {
        float mean_data_inv = 1.0/mean_data;
        long int i;
        for (i = 0; i < size; i++)
        {
            output[i] *= mean_data_inv;
        }
    }
    
    float metrics = sum_array(output, (int)size);
    if (size > 0)
        metrics /= size;
    
    float acc = 1.0 - metrics;
    
    free(output);
    
    return acc;
}

float
compute_metrics_MSEmask(float *X, float *y, float *pred, long int size)
{
    // [metrics], MSEmask
    
#define EPS 1e-15
    
    mult_arrays(pred, X, (int)size);
    
    float sum2 = 0.0;
    long int i;
    for (i = 0; i < size; i++)
    {
        float diff = y[i] - pred[i];

        sum2 += diff*diff;
    }
    if (sum2 < EPS)
        return 1.0;
    
    //float metrics = sqrt(sum2/size); // orig
    
    // Divide by 2, since batchnorm layer produces values in [-1, 1]
    float metrics = 0.5*sum2/size; // new
    
#if FIX_NEGATIVE_METRICS
    if (metrics > 1.0)
        metrics = 1.0;
#endif
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_MSEmask2(float *X, float *y, float *pred, long int size)
{
    bl_normalize_chan2(pred, IMAGE_CHANS, (int)size/IMAGE_CHANS);
    
    //
    mult_arrays(pred, X, (int)size);
    
    float sum = 0.0;
    int i;
    for(i = 0; i < size; ++i)
    {
        float diff = y[i] - pred[i];
        sum += diff * diff;
    }
    
    if (sum < EPS)
        return 1.0;
    
    // Divide by 2, since batchnorm layer produces values in [-1, 1]
    float metrics = 0.5*sum/size; // new
    
#if FIX_NEGATIVE_METRICS
    if (metrics > 1.0)
        metrics = 1.0;
#endif
    
    float acc = 1.0 - metrics;
    
    return acc;
}

// NEW
float
compute_metrics_L1mask(float *X, float *y, float *pred, long int size)
{
#define EPS 1e-15
    
    mult_arrays(pred, X, (int)size);
    
    float sum = 0.0;
    long int i;
    for (i = 0; i < size; i++)
    {
        float diff = y[i] - pred[i];
        
        sum += fabs(diff);
    }
    if (sum < EPS)
        return 1.0;
    
    // Divide by 2, since batchnorm layer produces values in [-1, 1]
    float metrics = 0.5*sum/size;
    
#if FIX_NEGATIVE_METRICS
    if (metrics > 1.0)
        metrics = 1.0;
#endif
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_L1mask2(float *X, float *y, float *pred, long int size)
{
#define EPS 1e-15
    
    bl_normalize_chan2(pred, IMAGE_CHANS, (int)size/IMAGE_CHANS);
    
    mult_arrays(pred, X, (int)size);
    
    float sum = 0.0;
    long int i;
    for (i = 0; i < size; i++)
    {
        float diff = y[i] - pred[i];
        
        sum += fabs(diff);
    }
    if (sum < EPS)
        return 1.0;
    
    // Divide by 2, since batchnorm layer produces values in [-1, 1]
    float metrics = 0.5*sum/size;
    
#if FIX_NEGATIVE_METRICS
    if (metrics > 1.0)
        metrics = 1.0;
#endif
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_MSE(float *X, float *y, float *pred, long int size)
{
    // [metrics], MSE
    
    //mult_arrays(pred, X, (int)size);
    
    float sum2 = 0.0;
    long int i;
    for (i = 0; i < size; i++)
    {
        float diff = y[i] - pred[i];
        
        sum2 += diff*diff;
    }
    if (sum2 < EPS)
        return 1.0;
    
    float metrics = sqrt(sum2/size);
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_accuracy(float *X, float *y, float *pred, long int size)
{
    // [metrics], accuracy
    float num_matches = 0.0;
    for(int i = 0; i < size; ++i)
    {
        float diff = fabs(y[i] - pred[i]);
        //if (diff < 0.5)
        //    num_matches++;
        //if (diff < EPS)
        //    num_matches++;
        if (diff > 1.0)
            diff = 1.0;
        
        num_matches += 1.0 - diff;
    }
    
    float acc = 0.0;
    if (size > 0)
        acc = ((float)num_matches)/size;
    
    return acc;
}

float
compute_score(network *net, char *mixfile0, char *mixfile1,
              char *stemfile0, char *stemfile1, char *stemfile2, char *stemfile3,
              int mode, long int index)
{
#if USE_STEREO_WIDTH
    float *dataNoSt;
    data data0 = random_data_samples2(mixfile0, mixfile1,
                                      stemfile0, stemfile1, stemfile2, stemfile3,
                                      mode, index, 1, 0/*, NULL*/, &dataNoSt);
#else
    data data0 = random_data_samples2(mixfile0, mixfile1,
                                      stemfile0, stemfile1, stemfile2, stemfile3,
                                      mode, index, 1, 0/*, NULL*/, NULL);
#endif
    
    float *X = data0.X.vals[0];
    float *y = data0.y.vals[0];
    
#if USE_STEREO_WIDTH
#if FIX_INPUT_DATA
    memcpy(net->input_data, dataNoSt, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
#endif
#else
    memcpy(net->input_data, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
#endif
    
    float *pred = network_predict(net, X);
    
    //long int size = IMAGE_WIDTH*IMAGE_HEIGHT*;
    long int size = IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS;
    
#if !METRICS_MSE_MASK
    //float metrics = compute_metrics_l1mask_norm(X, y, pred, size);
#else
    //float metrics = compute_metrics_MSEmask(X, y, pred, size);
    //float metrics = compute_metrics_MSEmask2(X, y, pred, size);
    float metrics = compute_metrics_MSEmask2(dataNoSt, y, pred, size);
#endif
    
#if !USE_BINARY_MASKS
#if METRICS_L1_MASK
    //float metrics = compute_metrics_L1mask(X, y, pred, size);
    //float metrics = compute_metrics_L1mask2(X, y, pred, size);
    
#if USE_STEREO_WIDTH
    float metrics = compute_metrics_L1mask2(dataNoSt, y, pred, size);
#else
    float metrics = compute_metrics_L1mask2(X, y, pred, size);
#endif
#endif
#endif

#if USE_BINARY_MASKS
    float metrics = compute_metrics_accuracy(X, y, pred, size);
#endif
    
    //float metrics = compute_metrics_MSE(X, y, pred, size);
    
    free_data(data0);

#if USE_STEREO_WIDTH
    free(dataNoSt);
#endif
    
    return metrics;
}

float
compute_score_avg(network *net, char *mixfile0, char *mixfile1,
                  char *stemfile0, char *stemfile1, char *stemfile2, char *stemfile3,
                  int mode)
{
    long int num_data = get_num_data_samples(mixfile0);
    
    long int num_test_data;
    if (mode == MODE_TRAIN)
    {
        num_test_data = num_data*TRAIN_RATIO;
    }
    else
    {
        num_test_data = num_data*(1.0 - TRAIN_RATIO);
    }
    
    num_test_data *= NUM_DATA_TEST_RATIO;
    
    long int i;
    float score = 0.0;
    for (i = 0; i < num_test_data; i++)
    {
        if (i >= num_data)
            break;
        
        float s = compute_score(net, mixfile0, mixfile1,
                                stemfile0, stemfile1, stemfile2, stemfile3,
                                mode, i);
        
        score += s;
    }
    
    if (num_test_data > 0)
        score /= num_test_data;
    
    return score;
}

void
dump_scores(char *cfgfile, char *weightfile,
            char *mixfile0,char *mixfile1,
            char *stemfile0, char *stemfile1, char *stemfile2, char *stemfile3,
            float *avg_score_train, float *avg_score_test)
{
    //
    network *net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    
    float score_train = compute_score_avg(net, mixfile0, mixfile1,
                                          stemfile0, stemfile1, stemfile2, stemfile3,
                                          MODE_TRAIN);
    float score_test = compute_score_avg(net, mixfile0, mixfile1,
                                         stemfile0, stemfile1, stemfile2, stemfile3,
                                         MODE_TEST);
    
    if ((avg_score_train != NULL) &&
        (avg_score_test != NULL))
    {
        if (*avg_score_train < 0) *avg_score_train = score_train;
        *avg_score_train = *avg_score_train*.9 + score_train*.1;
        score_train = *avg_score_train;
        
        if (*avg_score_test < 0) *avg_score_test = score_test;
        *avg_score_test = *avg_score_test*.9 + score_test*.1;
        score_test = *avg_score_test;
    }
    
    //
    char log_train[256];
    sprintf(log_train, TRAIN_DIR"train-score_train.txt");
    bl_dump_logs(score_train, log_train);
    
    char log_test[256];
    sprintf(log_test, TRAIN_DIR"train-score_test.txt");
    bl_dump_logs(score_test, log_test);
    
    if (!_darknet_quiet_flag)
        printf("Scores: train: %f test: %f\n", score_train, score_test);
    
    free_network(net);
}

void
rebalance_dump_dataset(char *datacfg, char *cfgfile, int start_datanum, int end_datanum)
{
    list *options = read_data_cfg(datacfg);
    
    char *mixfile0 = option_find_str(options, "train0", "");
    //char *mixfile1 = option_find_str(options, "train1", "");
    
    char *stemfile0 = option_find_str(options, "valid0", "");
    char *stemfile1 = option_find_str(options, "valid1", "");
    char *stemfile2 = option_find_str(options, "valid2", "");
    char *stemfile3 = option_find_str(options, "valid3", "");
    
    long int num_data_samples = get_num_data_samples(mixfile0);
    
    if (end_datanum == -1)
        end_datanum = (int)num_data_samples - 1;
    
    if (end_datanum >= num_data_samples)
        end_datanum = (int)num_data_samples - 1;
    
    for (int i = start_datanum; i <= end_datanum; i++)
    {
        images stem0 = load_one_image(stemfile0, i);
        images stem1 = load_one_image(stemfile1, i);
        images stem2 = load_one_image(stemfile2, i);
        images stem3 = load_one_image(stemfile3, i);
        
        float *y = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        memcpy(&y[0*IMAGE_WIDTH*IMAGE_HEIGHT], stem0.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        memcpy(&y[1*IMAGE_WIDTH*IMAGE_HEIGHT], stem1.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        memcpy(&y[2*IMAGE_WIDTH*IMAGE_HEIGHT], stem2.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        memcpy(&y[3*IMAGE_WIDTH*IMAGE_HEIGHT], stem3.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        
        // NEW
#if INPUTS_TO_DB
        amp_to_db(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        
        // DEBUG
        //swap_channels(y, IMAGE_WIDTH*IMAGE_HEIGHT, 4);
        
        char data_img_fname[256];
        if (i < 10)
            sprintf(data_img_fname, DUMP_DATASET_DIR"data-00%d", i);
        else if (i < 100)
            sprintf(data_img_fname, DUMP_DATASET_DIR"data-0%d", i);
        else
            sprintf(data_img_fname, DUMP_DATASET_DIR"data-%d", i);
        
        //bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, truth_img_fname, 1);
        bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, data_img_fname);
        
        free_images(stem0);
        free_images(stem1);
        free_images(stem2);
        free_images(stem3);
        
        free(y);
    }
}

void
test_rebalance(char *datacfg, char *cfgfile, char *weightfile,
               int backup_num, const char *score_filename)
{
    _darknet_quiet_flag = 0;
    
    list *options = read_data_cfg(datacfg);
    
    char *mixfile0 = option_find_str(options, "train0", "");
    
#if USE_STEREO_WIDTH
    char *mixfile1 = option_find_str(options, "train1", "");
#endif
    
    char *stemfile0 = option_find_str(options, "valid0", "");
    char *stemfile1 = option_find_str(options, "valid1", "");
    char *stemfile2 = option_find_str(options, "valid2", "");
    char *stemfile3 = option_find_str(options, "valid3", "");
    
    long int num_data_samples = get_num_data_samples(mixfile0);
    
    // TEST DATA
    long int test_data_num = num_data_samples*0.95;
    //long int test_data_num = num_data_samples*0.4;
    
    if (DEBUG_FORCE_SAMPLE_NUM_TEST >= 0)
        test_data_num = DEBUG_FORCE_SAMPLE_NUM_TEST;
    
    images mix0 = load_one_image(mixfile0, test_data_num);

#if USE_STEREO_WIDTH
    images mix1 = load_one_image(mixfile1, test_data_num);
#endif
    
    images stem0 = load_one_image(stemfile0, test_data_num);
    images stem1 = load_one_image(stemfile1, test_data_num);
    images stem2 = load_one_image(stemfile2, test_data_num);
    images stem3 = load_one_image(stemfile3, test_data_num);

    // NOTE: would be better to put this in "dump_dataset()"
#if 1 // #1
    // Dump mix as binary (for debugging later)
    char mix_bin_fname[256];
    sprintf(mix_bin_fname, TRAIN_DIR"mix");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, mix0.data, mix_bin_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, mix0.data, mix_bin_fname);

    char stem0_bin_fname[256];
    sprintf(stem0_bin_fname, TRAIN_DIR"stem0");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem0.data, stem0_bin_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem0.data, stem0_bin_fname);

    char stem1_bin_fname[256];
    sprintf(stem1_bin_fname, TRAIN_DIR"stem1");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem1.data, stem1_bin_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem1.data, stem1_bin_fname);

    char stem2_bin_fname[256];
    sprintf(stem2_bin_fname, TRAIN_DIR"stem2");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem2.data, stem2_bin_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem2.data, stem2_bin_fname);

    char stem3_bin_fname[256];
    sprintf(stem3_bin_fname, TRAIN_DIR"stem3");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem3.data, stem3_bin_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, stem3.data, stem3_bin_fname);
#endif
    
    //
    srand(111111111);
    network *net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    srand(2222222);

#if DEBUG_TEST_NET
    //bl_dbg_save_weights(net, "dbg-data");
    char dbg_suffix[255];
    if (backup_num < 0 )
        sprintf(dbg_suffix, "");
    else
        sprintf(dbg_suffix, "_%d", backup_num);
    
    bl_dbg_save_data(net, "dbg-data", "test-", dbg_suffix,
                     DEBUG_DUMP_DATA, DEBUG_DUMP_WEIGHTS, DEBUG_DUMP_TXT);
#endif
    
    double time0 = what_time_is_it_now();
    
    float *X = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    // OLD: this looks very wierd when using dB scale
    //memcpy(&X[0*IMAGE_WIDTH*IMAGE_HEIGHT], mix.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    //memcpy(&X[1*IMAGE_WIDTH*IMAGE_HEIGHT], mix.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    //memcpy(&X[2*IMAGE_WIDTH*IMAGE_HEIGHT], mix.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    //memcpy(&X[3*IMAGE_WIDTH*IMAGE_HEIGHT], mix.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    float *y = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    memcpy(&y[0*IMAGE_WIDTH*IMAGE_HEIGHT], stem0.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    memcpy(&y[1*IMAGE_WIDTH*IMAGE_HEIGHT], stem1.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    memcpy(&y[2*IMAGE_WIDTH*IMAGE_HEIGHT], stem2.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    memcpy(&y[3*IMAGE_WIDTH*IMAGE_HEIGHT], stem3.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    // DEBUG
    //swap_channels(y, IMAGE_WIDTH*IMAGE_HEIGHT, 4);
    
//#if INPUTS_TO_DB
//    //amp_to_db(X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
//    amp_to_db(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
//#endif
    
#if X_IS_STEM_SUM
    // NEW: this is different, and looks better!
    for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT; i++)
    {
        X[i] = y[i + 0*IMAGE_WIDTH*IMAGE_HEIGHT];
        X[i] += y[i + 1*IMAGE_WIDTH*IMAGE_HEIGHT];
        X[i] += y[i + 2*IMAGE_WIDTH*IMAGE_HEIGHT];
        X[i] += y[i + 3*IMAGE_WIDTH*IMAGE_HEIGHT];
    
        X[i + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
        X[i + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
        X[i + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
    }
#else
    for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT; i++)
    {
        X[i] = mix0.data[i];
        X[i + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
        X[i + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
        X[i + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
    }
    // NEW
    //bl_normalize(X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
    
#if 1 // #2
    // Dump mix as binary (for debugging later)
    char mix_bin_sum_fname[256];
    sprintf(mix_bin_sum_fname, TRAIN_DIR"mix-sum");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, X, mix_bin_sum_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, X, mix_bin_sum_fname);
#endif
    
#if INPUTS_TO_DB
    amp_to_db(X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    amp_to_db(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
    
    
#if NORMALIZE_INPUTS
    bl_normalize2(X, y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS); // origin
    // TODO: normalize by channel?
#endif
#if NORMALIZE_INPUTS2
    //bl_normalize(X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
#if !USE_BINARY_MASKS
    bl_normalize(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
#endif
    
#if NORMALIZE_INPUTS3
    bl_normalize_chan(X, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
    bl_normalize_chan(y, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif

#if DEBUG_METRICS
    net->dbg_truth = y;
    memcpy(net->input_data, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
#endif
    
#if USE_BINARY_MASKS
    char nobin_img_fname[256];
    sprintf(nobin_img_fname, TRAIN_DIR"test-truth-nobin");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, nobin_img_fname);
    
    bl_binarize(y, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
    
    // For stereo
    float *Xst = X;
    
#if USE_STEREO_WIDTH
    Xst = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    memcpy(Xst, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    
    // Set stereo width
    memcpy(&Xst[1*IMAGE_WIDTH*IMAGE_HEIGHT], mix1.data, IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    // Normalize stereo width
    bl_normalize(&Xst[1*IMAGE_WIDTH*IMAGE_HEIGHT], IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
    
    float *pred = network_predict(net, Xst);
    
#if !USE_BINARY_MASKS
    // NOTE: necessary, to multiply like in [metrics]
    // (when global=1)
    // Without this, there is a sort of color fog/layer in black areas
    bl_normalize(pred, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS); // NEW
#endif
    
    if (!_darknet_quiet_flag)
    {
        double predictDuration = what_time_is_it_now() - time0;
        printf("Predicted in %f seconds.\n", predictDuration);
    }
    
#if !USE_BINARY_MASKS
    // mask mult
    float *mask_mult = calloc(IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS, sizeof(float));
    memcpy(mask_mult, pred, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    mult_arrays(mask_mult, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    char mask_mult_txt_fname[256];
    sprintf(mask_mult_txt_fname, TRAIN_DIR"test-mask_mult");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, mask_mult, mask_mult_txt_fname);
    
    char mask_mult_img_fname[256];
    sprintf(mask_mult_img_fname, TRAIN_DIR"test-mask_mult");
    //bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, mask_mult, mask_mult_img_fname, 1);
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, mask_mult, mask_mult_img_fname);
    
    free(mask_mult);
#endif
    
    //
    char input_txt_fname[256];
    sprintf(input_txt_fname, TRAIN_DIR"test-input");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, Xst, input_txt_fname);
    
    char truth_txt_fname[256];
    sprintf(truth_txt_fname, TRAIN_DIR"test-truth");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, truth_txt_fname);
    
    char pred_txt_fname[256];
    sprintf(pred_txt_fname, TRAIN_DIR"test-pred");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, pred, pred_txt_fname);
    
    //
    //
    char input_img_fname[256];
    sprintf(input_img_fname, TRAIN_DIR"test-input");
    bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, Xst, input_img_fname, 1);
    //bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, X, input_img_fname);
    
    char truth_img_fname[256];
    sprintf(truth_img_fname, TRAIN_DIR"test-truth");
    //bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, truth_img_fname, 1);
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, truth_img_fname);
    
    char pred_img_fname[256];
    sprintf(pred_img_fname, TRAIN_DIR"test-pred");
    //bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, pred, pred_img_fname, 1);
    
    //bl_normalize_chan2(pred, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT/IMAGE_CHANS); // TEST
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, pred, pred_img_fname);
    
    //long int size = IMAGE_WIDTH*IMAGE_HEIGHT;
    long int size = IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS;
    
#if !METRICS_MSE_MASK
    //float score = compute_metrics_l1mask_norm(X, y, pred, size);
#else
    //float score = compute_metrics_MSEmask(X, y, pred, size);
    float score = compute_metrics_MSEmask2(X, y, pred, size);
#endif
    
#if METRICS_L1_MASK
    //float score = compute_metrics_L1mask(X, y, pred, size);
    float score = compute_metrics_L1mask2(X, y, pred, size);
#endif
    
#if USE_BINARY_MASKS
    float score = compute_metrics_accuracy(X, y, pred, size);
#endif
    
#if USE_BINARY_MASKS
    bl_binarize(pred, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);

    char pred_bin_img_fname[256];
    sprintf(pred_bin_img_fname, TRAIN_DIR"test-pred-bin");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, pred, pred_bin_img_fname);
#endif
    
    //float score = compute_metrics_MSE(X, y, pred, IMAGE_WIDTH*IMAGE_HEIGHT);
    fprintf(stderr, "score: %f\n", score);
    
    if ((score_filename != NULL) && (strlen(score_filename) > 0))
    {
        FILE *score_file = fopen(score_filename, "ab");
        fprintf(score_file, "%f ", score);
        fclose(score_file);
    }
    
    //
    free(X);
    free(y);
    
#if USE_STEREO_WIDTH
    free(Xst);
#endif
    
    free_images(mix0);
    
#if USE_STEREO_WIDTH
    free_images(mix1);
#endif
    
    free_images(stem0);
    free_images(stem1);
    free_images(stem2);
    free_images(stem3);
    
    free_network(net);
    
    free_list(options);
}

void
train_rebalance(char *datacfg, char *cfgfile, char *weightfile,
                int *gpus, int ngpus, int clear)
{
    _darknet_quiet_flag = 0;
    
    list *options = read_data_cfg(datacfg);
    char *backup_directory = option_find_str(options, "backup", "./backup/");
    
    char *mixfile0 = option_find_str(options, "train0", "");
    
#if USE_STEREO_WIDTH
    char *mixfile1 = option_find_str(options, "train1", "");
#else
    char *mixfile1 = NULL;
#endif
    
    char *stemfile0 = option_find_str(options, "valid0", "");
    char *stemfile1 = option_find_str(options, "valid1", "");
    char *stemfile2 = option_find_str(options, "valid2", "");
    char *stemfile3 = option_find_str(options, "valid3", "");
    
    srand(111111111);
    
    char *base = basecfg(cfgfile);
    
    if (!_darknet_quiet_flag)
        printf("%s\n", base);
    
    float avg_loss = -1;
    float avg_acc = -1;
    
    float avg_score_train = -1;
    float avg_score_test = -1;
    
    float sum_loss = 0.0;
    float sum_acc = 0.0;
    
    // Create the network
    network *net = load_network(cfgfile, weightfile, clear);
    
    if (!_darknet_quiet_flag)
        printf("Learning Rate: %g, Momentum: %g, Decay: %g\n",
               net->learning_rate, net->momentum, net->decay);
    
    long int num_data = get_num_data_samples(mixfile0);
    
    long int num_epochs = net->max_batches/num_data;
    long int epoch_num = 1;
    double epoch_time = what_time_is_it_now();
    
    srand(111111111);
    generate_shuffle_map(&_full_shuffle_map, num_data);
    
    generate_shuffle_map(&_train_shuffle_map, num_data*TRAIN_RATIO);
    generate_shuffle_map(&_test_shuffle_map, num_data*(1.0 - TRAIN_RATIO));
    
    // Main loop
    long int train_index = 0;
    long int sample_index = 0;
    double sleep_time = what_time_is_it_now();
    while(get_current_batch(net) < net->max_batches)
    {
#if DEBUG_TRAIN_NET
        char dbg_suffix[255];
        sprintf(dbg_suffix, "_%ld-%ld", epoch_num, sample_index);
        
        bl_dbg_save_data(net, "dbg-data", "train-", dbg_suffix,
                         DEBUG_DUMP_DATA, DEBUG_DUMP_WEIGHTS, DEBUG_DUMP_TXT);
#endif
        
        //int source_silence = 0;
        double time0 = what_time_is_it_now();
        
#if USE_STEREO_WIDTH
        float *dataNoSt;
        data train = random_data_samples2(mixfile0, mixfile1,
                                         stemfile0, stemfile1, stemfile2, stemfile3,
                                         MODE_TRAIN, (int)train_index, net->batch,
                                         DATA_AUGMENTATION, &dataNoSt); //, &source_silence);
#else
        data train = random_data_samples2(mixfile0, mixfile1,
                                          stemfile0, stemfile1, stemfile2, stemfile3,
                                          MODE_TRAIN, (int)train_index, net->batch,
                                          DATA_AUGMENTATION, NULL);
#endif
        
        /*if (source_silence)
        {
            train_index++;
            
            continue;
        }*/
        
        //train_index++;
        //sample_index++;
        
        train_index += net->batch;
        sample_index += net->batch;
        
        time0 = what_time_is_it_now();
        
#if USE_STEREO_WIDTH
#if FIX_INPUT_DATA
        memcpy(net->input_data, dataNoSt, net->batch*IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
#endif
#else
        //memcpy(net->input_data, train.X.vals[0], net->batch*IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
#endif
        // Train
        float loss = train_network(net, train);
        
        // Loss
        if (avg_loss < 0) avg_loss = loss;
        avg_loss = avg_loss*.9 + loss*.1;
        sum_loss += loss;
        
        // Acc
        float acc = 0.0;
        if (net->metrics)
        {
            acc = net->metrics[1];
        }
        if (avg_acc < 0) avg_acc = acc;
        avg_acc = avg_acc*.9 + acc*.1;
        sum_acc += acc;
        
        //
        //long current_batch = get_current_batch(net);
        long int num_data_per_epoch = num_data*TRAIN_RATIO*NUM_DATA_EPOCH_RATIO;
        
        if (DEBUG_FORCE_SAMPLE_NUM >= 0)
            num_data_per_epoch = DEBUG_FORCE_SAMPLE_NUM_SAMPLES;
        
        //
        double epoch_time0 = what_time_is_it_now();
        double elapsed = epoch_time0 - epoch_time;
        
        //long int step_num = current_batch % num_data_by_epoch;
        if (train_index > 0)//(step_num > 0)
        {
            double eta = 0.0;
            double eta_total_time = elapsed*((double)num_data_per_epoch)/train_index;
            if (num_data > 0)
                eta = (1.0 - ((double)train_index)/num_data_per_epoch)*eta_total_time;
        
            if (!_darknet_quiet_flag)
            {
                printf("\r%ld/%ld: epoch: %ld/%ld, lr: %f, loss: %f, acc: %f, eta: %.1lfs",
                       train_index,
                       num_data_per_epoch,
                       epoch_num, num_epochs,
                       net->learning_rate,
                       avg_loss, avg_acc, eta);
                fflush(stdout);
            }
        }
        
        //if (train_index % num_data_by_epoch == 0)
        // ORIG: what ifit remains less samples than batch size ?
        //if (train_index >= num_data_per_epoch)
        // FIX: squeeze last samples if it remains less samples than batch size
        if (train_index + net->batch - 1 >= num_data_per_epoch)
        // Epoch end
        {
            if (!_darknet_quiet_flag)
            {
                printf("\r%ld/%ld: epoch: %ld/%ld, lr: %f, loss: %f, acc: %f, elaps: %.1lfs",
                       train_index,
                       num_data_per_epoch,
                       epoch_num, num_epochs,
                       net->learning_rate,
                       avg_loss, avg_acc, elapsed);
                fflush(stdout);
            
                printf("\n");
            }
            
            //
            _darknet_quiet_flag = 1;
            
            char buff[256];
            sprintf(buff, "%s/%s.backup", backup_directory, base);
            save_weights(net, buff);
            
            //
            char buff2[256];
            sprintf(buff2, "%s/%s_%d.weights", backup_directory, base, (int)epoch_num);
            save_weights(net, buff2);
            
            _darknet_quiet_flag = 0;
            
            if (num_data > 0)
            {
                char log_loss[256];
                sprintf(log_loss, TRAIN_DIR"train_loss.txt");
                bl_dump_logs(sum_loss/num_data_per_epoch, log_loss);
                
                char log_acc[256];
                sprintf(log_acc, TRAIN_DIR"train_acc.txt");
                bl_dump_logs(sum_acc/num_data_per_epoch, log_acc);
                
                sum_loss = 0.0;
                sum_acc = 0.0;
            }
            
            epoch_num++;
            epoch_time = what_time_is_it_now();
            
            //if (DEBUG_FORCE_SAMPLE_NUM >= 0) // HACK for debug
            //{
            _darknet_quiet_flag = 1;
            // WARNINH: this makes an undefined behavior, when setting net batch to 1,
            // after having loaded with 20 batches in cfg file
            dump_scores(cfgfile, buff,
                        mixfile0, mixfile1,
                        stemfile0, stemfile1, stemfile2, stemfile3,
                        &avg_score_train, &avg_score_test);
            _darknet_quiet_flag = 0;
            //}
            
            // Regenerate train and test orders
            train_index = 0;
            generate_shuffle_map(&_train_shuffle_map, num_data*TRAIN_RATIO);
            generate_shuffle_map(&_test_shuffle_map, num_data*(1.0 - TRAIN_RATIO));
            
#ifndef GPU // Don't sleep on big ovh computer!
            // Sleep ?
            //if (current_batch % TRAIN_SLEEP_EVERY == 0)
            double time0 = what_time_is_it_now();;
            if (time0 - sleep_time > TRAIN_SLEEP_EVERY)
            {
                // Avoid sleeping in the middle of an epoch
                if (!_darknet_quiet_flag)
                    printf("\nSleeping %ds...\n", TRAIN_SLEEP_TIME);
                    
                sleep(TRAIN_SLEEP_TIME);
                    
                sleep_time = time0;
                
                epoch_time = what_time_is_it_now();
            }
#endif
        }
        
        free_data(train);
    }
    
    char buff[256];
    sprintf(buff, "%s/%s_final.weights", backup_directory, base);
    save_weights(net, buff);
    
    free_network(net);
    
    free(base);
    free_list(options);
}

void
validate_rebalance(char *datacfg, char *cfgfile, char *weightfile, char *outfile)
{
    // Not done...
}

void
run_rebalance(int argc, char **argv)
{
    if(argc < 4)
    {
        fprintf(stderr, "usage: %s %s [train/test/valid] [cfg] [weights (optional)]\n", argv[0], argv[1]);
        
        return;
    }
    
    char *gpu_list = find_char_arg(argc, argv, "-gpus", 0);
    char *outfile = find_char_arg(argc, argv, "-out", 0);
    int *gpus = 0;
    int gpu = 0;
    int ngpus = 0;
    if(gpu_list){
        printf("%s\n", gpu_list);
        int len = (int)strlen(gpu_list);
        ngpus = 1;
        int i;
        for(i = 0; i < len; ++i){
            if (gpu_list[i] == ',') ++ngpus;
        }
        gpus = calloc(ngpus, sizeof(int));
        for(i = 0; i < ngpus; ++i){
            gpus[i] = atoi(gpu_list);
            gpu_list = strchr(gpu_list, ',')+1;
        }
    } else {
        gpu = gpu_index;
        gpus = &gpu;
        ngpus = 1;
    }
    
    int clear = find_arg(argc, argv, "-clear");
    
    char *datacfg = argv[3];
    char *cfg = argv[4];
    char *weights = (argc > 5) ? argv[5] : 0;
    
    if(0==strcmp(argv[2], "dump-dataset"))
    {
        int start_datanum = find_int_arg(argc, argv, "-start", 0);
        int end_datanum = find_int_arg(argc, argv, "-end", -1);
        
        rebalance_dump_dataset(datacfg, cfg, start_datanum, end_datanum);
    }
    else if(0==strcmp(argv[2], "test"))
    {
        int backup_num = find_int_arg(argc, argv, "-backup-num", -1);
        char *score_filename = find_char_arg(argc, argv, "-score-file", "");
        
        test_rebalance(datacfg, cfg, weights, backup_num, score_filename);
    }
    else if(0==strcmp(argv[2], "train"))
            train_rebalance(datacfg, cfg, weights, gpus, ngpus, clear);
    else if(0==strcmp(argv[2], "valid"))
            validate_rebalance(datacfg, cfg, weights, outfile);
}

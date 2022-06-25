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

#define IMAGE_WIDTH  256
#define IMAGE_HEIGHT 32
#define IMAGE_CHANS  4

// Train
#define TRAIN_RATIO 0.5 //0.8

#define TRAIN_DIR "training/"
#define DUMP_DATASET_DIR "dataset/"

// Sleep every 5mn
#define TRAIN_SLEEP_EVERY 300
// Sleep 60 seconds
#define TRAIN_SLEEP_TIME 60

#define NUM_DATA_EPOCH_RATIO 0.1
#define NUM_DATA_TEST_RATIO NUM_DATA_EPOCH_RATIO*0.1

#define DEBUG_DUMP_DATA 1
#define DEBUG_DUMP_WEIGHTS 1
#define DEBUG_DUMP_TXT 1 //0 //1

// Instance num
// See: https://www.researchgate.net/publication/320966887_Image-to-Image_Translation_with_Conditional_Adversarial_Networks#pfe
//#define USE_INSTANCE_NORM_METHOD 1
//#define INST_NORM_NUM_TEST_DATA 100

//
#define MODE_TRAIN 0
#define MODE_TEST  1

// Use separate mask?
// (or just don't use mask, and compute directly the image)
#define METRICS_L1_MASK 0 //1

// "A-training-nodown-bin-long"
//#define USE_BINARIZATION 1
//#define USE_AMP2DB60 0
//#define USE_JANSON_EX 1 // Gives more continuous vocal partials in truth-bin image, when using binarization
//#define TRUTH_IS_MASK 0

// Current (same as prev, but nobin)
//#define USE_BINARIZATION 0
//#define USE_AMP2DB60 1
//#define USE_JANSON_EX 0
//#define TRUTH_IS_MASK 1


// Binarize data?
#define USE_BINARIZATION 0 //1 //0 //1 // Orig: 1

#define USE_AMP2DB60 1 //0 //1// Orig 0
#define USE_JANSON_EX 0 // Orig: 1

// Truth is sum of stems, or truth is mask to be multiplied by mix?
#define TRUTH_IS_MASK 1 // Orig: 0

// NEW
#define USE_SIGMO_CONTRAST 1
#define SIGMO_ALPHA 0.1 // 0.2 makes appear drums more

// TESTS
/*#define USE_BINARIZATION 1
#define USE_BIN_SIGMOID 1 // TEST
#define USE_JANSON_EX 1 */
//

#define TEST_DATA_NUM 2005 // For mel3 + overlap
//#define TEST_DATA_NUM 2012 // Voice + Drums

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

void
amp_to_db60(float *buf, int size)
{
    for (int i = 0; i < size; i++)
    {
        float v = buf[i];
        v = bl_amp_to_db_norm60(v);
        buf[i] = v;
    }
}

void
stems_to_mask(float *y, const float *X, int size, int nchan)
{
    for (int i = 0; i < size; i++)
    {
        float x = X[i];
        
        if (x > EPS)
        {
            float xinv = 1.0/x;
            for (int j = 0; j < nchan; j++)
            {
                y[i + j*size] *= xinv;
            }
        }
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
    memcpy(&img_data[chan_size*CHAN0], &img_data[chan_size*CHAN1],
           chan_size*sizeof(float));
    
    // Copy tmp to 2nd channel
    memcpy(&img_data[chan_size*CHAN1], tmp, chan_size*sizeof(float));
    
    free(tmp);
}

data
random_data_samples(const char *mixfile,
                    const char *stemfile0, const char *stemfile1,
                    const char *stemfile2, const char *stemfile3,
                    int mode, long int start_index, long int num_data)
{
    // Result
    data d = {0};
    d.X = make_matrix((int)num_data, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    d.y = make_matrix((int)num_data, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    // Get sample index
    long int total_num_data = get_num_data_samples(mixfile);
    
    long int start_data = 0;
    if (mode == MODE_TEST)
        start_data = total_num_data*TRAIN_RATIO;
    
    FILE *mixfp0 = fopen(mixfile, "rb");
    
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
        
        // Set file position
        long int file_pos = index0*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float);

        // X
        fseek(mixfp0, file_pos, SEEK_SET);

        // Y
        fseek(stemfp0, file_pos, SEEK_SET);
        fseek(stemfp1, file_pos, SEEK_SET);
        fseek(stemfp2, file_pos, SEEK_SET);
        fseek(stemfp3, file_pos, SEEK_SET);

        // Y
        fread(&d.y.vals[i][0*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp0);
        fread(&d.y.vals[i][1*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp1);
        fread(&d.y.vals[i][2*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp2);
        fread(&d.y.vals[i][3*IMAGE_WIDTH*IMAGE_HEIGHT],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, stemfp3);

        // X
        fread(&d.X.vals[i][0],
              sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, mixfp0);
        
        for (int j = 0; j < IMAGE_WIDTH*IMAGE_HEIGHT; j++)
        {
            d.X.vals[i][j + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
            d.X.vals[i][j + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = d.X.vals[i][j];
        }
        
        // New normalization (see Janson 2017)
        float norm = bl_max(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        if (norm > 0.0)
        {
            bl_mult2(d.X.vals[i], 1.0f/norm, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
            
#if USE_JANSON_EX
            bl_mult2(d.y.vals[i], 1.0f/norm, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        }
        
        amp_to_db(d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        
#if !USE_AMP2DB60
        amp_to_db(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#else
        amp_to_db60(d.y.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        
#if TRUTH_IS_MASK
        stems_to_mask(d.y.vals[i], d.X.vals[i], IMAGE_WIDTH*IMAGE_HEIGHT, IMAGE_CHANS);
#endif
        
#if USE_BINARIZATION
        // Binarize
#if !USE_BIN_SIGMOID
        bl_binarize(d.y.vals[i], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#else
        bl_sigmo_bin(d.y.vals[i], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
#endif
        
#if USE_SIGMO_CONTRAST
        bl_sigmo_contrast(d.y.vals[i], IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT, SIGMO_ALPHA);
#endif
    }
    
    fclose(mixfp0);
    
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
    
    long int file_pos = index*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float);
    
    FILE *fp = fopen(filename, "rb");
    
    fseek(fp, file_pos, SEEK_SET);
    fread(img.data, sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT, fp);
    
    fclose(fp);
    
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
    
    // Divide by 2, since batchnorm layer produces values in [-1, 1]
    float metrics = 0.5*sum2/size;
    
    if (metrics > 1.0)
        metrics = 1.0;
    
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
    float metrics = 0.5*sum/size;
    
    if (metrics > 1.0)
        metrics = 1.0;
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_L1mask(float *X, float *y, float *pred, long int size)
{    
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
    
    if (metrics > 1.0)
        metrics = 1.0;
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_L1mask2(float *X, float *y, float *pred, long int size)
{    
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
    
    if (metrics > 1.0)
        metrics = 1.0;
    
    float acc = 1.0 - metrics;
    
    return acc;
}

float
compute_metrics_MSE(float *X, float *y, float *pred, long int size)
{
    // [metrics], MSE
        
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
compute_score(network *net, char *mixfile,
              char *stemfile0, char *stemfile1, char *stemfile2, char *stemfile3,
              int mode, long int index)
{
    data data0 = random_data_samples(mixfile,
                                     stemfile0, stemfile1, stemfile2, stemfile3,
                                     mode, index, 1);
    
    float *X = data0.X.vals[0];
    float *y = data0.y.vals[0];
    
    memcpy(net->input_data, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    
    float *pred = network_predict(net, X);
    
    long int size = IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS;
    
#if METRICS_L1_MASK
    float metrics = compute_metrics_L1mask2(X, y, pred, size);
#else
    float metrics = compute_metrics_accuracy(X, y, pred, size);
#endif
    
    free_data(data0);
    
    return metrics;
}

float
compute_score_avg(network *net, char *mixfile,
                  char *stemfile0, char *stemfile1,
                  char *stemfile2, char *stemfile3,
                  int mode)
{
    long int num_data = get_num_data_samples(mixfile);
    
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
        
        float s = compute_score(net, mixfile,
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
            char *mixfile,
            char *stemfile0, char *stemfile1,
            char *stemfile2, char *stemfile3,
            float *avg_score_train, float *avg_score_test)
{
    //
    network *net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    
    float score_train = compute_score_avg(net, mixfile,
                                          stemfile0, stemfile1, stemfile2, stemfile3,
                                          MODE_TRAIN);
    float score_test = compute_score_avg(net, mixfile,
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
rebalance_dump_dataset(char *datacfg, char *cfgfile,
                       int start_datanum, int end_datanum)
{
    list *options = read_data_cfg(datacfg);
    
    char *mixfile = option_find_str(options, "train0", "");
    
    char *stemfile0 = option_find_str(options, "valid0", "");
    char *stemfile1 = option_find_str(options, "valid1", "");
    char *stemfile2 = option_find_str(options, "valid2", "");
    char *stemfile3 = option_find_str(options, "valid3", "");
    
    long int num_data_samples = get_num_data_samples(mixfile);
    
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
        memcpy(&y[0*IMAGE_WIDTH*IMAGE_HEIGHT], stem0.data,
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        memcpy(&y[1*IMAGE_WIDTH*IMAGE_HEIGHT], stem1.data,
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        memcpy(&y[2*IMAGE_WIDTH*IMAGE_HEIGHT], stem2.data,
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        memcpy(&y[3*IMAGE_WIDTH*IMAGE_HEIGHT], stem3.data,
               IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
        
#if !USE_AMP2DB60
        amp_to_db(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#else
        amp_to_db60(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
        
        char data_img_fname[256];
        if (i < 10)
            sprintf(data_img_fname, DUMP_DATASET_DIR"data-00%d", i);
        else if (i < 100)
            sprintf(data_img_fname, DUMP_DATASET_DIR"data-0%d", i);
        else
            sprintf(data_img_fname, DUMP_DATASET_DIR"data-%d", i);
        
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
    
    char *mixfile = option_find_str(options, "train0", "");
    
    char *stemfile0 = option_find_str(options, "valid0", "");
    char *stemfile1 = option_find_str(options, "valid1", "");
    char *stemfile2 = option_find_str(options, "valid2", "");
    char *stemfile3 = option_find_str(options, "valid3", "");
    
    long int num_data_samples = get_num_data_samples(mixfile);
    
    // 
    long int test_data_num = TEST_DATA_NUM; //num_data_samples*0.95;
    
    images mix0 = load_one_image(mixfile, test_data_num);
    
    images stem0 = load_one_image(stemfile0, test_data_num);
    images stem1 = load_one_image(stemfile1, test_data_num);
    images stem2 = load_one_image(stemfile2, test_data_num);
    images stem3 = load_one_image(stemfile3, test_data_num);

    // NOTE: would be better to put this in "dump_dataset()"
    
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
    
    //
    srand(111111111);
    network *net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    srand(2222222);

    char dbg_suffix[255];
    if (backup_num < 0 )
        sprintf(dbg_suffix, "");
    else
        sprintf(dbg_suffix, "_%d", backup_num);
    
    bl_dbg_save_data(net, "dbg-data", "test-", dbg_suffix,
                     DEBUG_DUMP_DATA, DEBUG_DUMP_WEIGHTS, DEBUG_DUMP_TXT);
    
    double time0 = what_time_is_it_now();
    
    float *X = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    float *y = calloc(sizeof(float), IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    memcpy(&y[0*IMAGE_WIDTH*IMAGE_HEIGHT], stem0.data,
           IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    memcpy(&y[1*IMAGE_WIDTH*IMAGE_HEIGHT], stem1.data,
           IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    memcpy(&y[2*IMAGE_WIDTH*IMAGE_HEIGHT], stem2.data,
           IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    memcpy(&y[3*IMAGE_WIDTH*IMAGE_HEIGHT], stem3.data,
           IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(float));
    
    for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT; i++)
    {
        X[i] = mix0.data[i];
        X[i + 1*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
        X[i + 2*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
        X[i + 3*IMAGE_WIDTH*IMAGE_HEIGHT] = X[i];
    }
    
    // Dump mix as binary (for debugging later)
    char mix_bin_sum_fname[256];
    sprintf(mix_bin_sum_fname, TRAIN_DIR"mix-sum");
    bl_save_image_bin(IMAGE_WIDTH, IMAGE_HEIGHT, 1, X, mix_bin_sum_fname);
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, 1, X, mix_bin_sum_fname);
    
    // New normalization (see Janson 2017)
    float norm = bl_max(X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    if (norm > 0.0)
    {
        bl_mult2(X, 1.0f/norm, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
        
#if USE_JANSON_EX
        bl_mult2(y, 1.0f/norm, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
    }

    amp_to_db(X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
#if !USE_AMP2DB60
    amp_to_db(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#else
    amp_to_db60(y, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
#endif
    
#if TRUTH_IS_MASK
    stems_to_mask(y, X, IMAGE_WIDTH*IMAGE_HEIGHT, IMAGE_CHANS);
#endif
    
    net->dbg_truth = y;
    memcpy(net->input_data, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    
    // NEW: should not be set to 0, no?
#if USE_BINARIZATION //0 //USE_BINARIZATION
    char nobin_img_fname[256];
    sprintf(nobin_img_fname, TRAIN_DIR"test-truth-nobin");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, nobin_img_fname);

#if !USE_BIN_SIGMOID
    // Binarize
    bl_binarize(y, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#else
    bl_sigmo_bin(y, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif

#endif
    
#if USE_SIGMO_CONTRAST
    bl_sigmo_contrast(y, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT, SIGMO_ALPHA);
#endif
    
    float *pred = network_predict(net, X);
        
    if (!_darknet_quiet_flag)
    {
        double predictDuration = what_time_is_it_now() - time0;
        printf("Predicted in %f seconds.\n", predictDuration);
    }
    
#if METRICS_L1_MASK
    // mask mult
    float *mask_mult = calloc(IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS, sizeof(float));
    memcpy(mask_mult, pred, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS*sizeof(float));
    mult_arrays(mask_mult, X, IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS);
    
    char mask_mult_txt_fname[256];
    sprintf(mask_mult_txt_fname, TRAIN_DIR"test-mask_mult");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                      mask_mult, mask_mult_txt_fname);
    
    char mask_mult_img_fname[256];
    sprintf(mask_mult_img_fname, TRAIN_DIR"test-mask_mult");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                     mask_mult, mask_mult_img_fname);
    
    free(mask_mult);
#endif
    
    //
    char input_txt_fname[256];
    sprintf(input_txt_fname, TRAIN_DIR"test-input");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, X, input_txt_fname);
    
    char truth_txt_fname[256];
    sprintf(truth_txt_fname, TRAIN_DIR"test-truth");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, truth_txt_fname);
    
    char pred_txt_fname[256];
    sprintf(pred_txt_fname, TRAIN_DIR"test-pred");
    bl_save_image_txt(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, pred, pred_txt_fname);
    
    //
    char input_img_fname[256];
    sprintf(input_img_fname, TRAIN_DIR"test-input");
    bl_save_image(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, X, input_img_fname, 1);
    
    char truth_img_fname[256];
    sprintf(truth_img_fname, TRAIN_DIR"test-truth");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, y, truth_img_fname);
    
    char pred_img_fname[256];
    sprintf(pred_img_fname, TRAIN_DIR"test-pred");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS, pred, pred_img_fname);
    
    long int size = IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANS;
    
#if METRICS_L1_MASK
    float score = compute_metrics_L1mask2(X, y, pred, size);
#else
    float score = compute_metrics_accuracy(X, y, pred, size);
    //float score = compute_metrics_accuracy(X, y, pred, size/4);
#endif

    fprintf(stderr, "score: %f\n", score);
    
#if 1 //USE_BINARIZATION
    
#if !USE_BIN_SIGMOID
    bl_binarize(pred, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#else
    bl_sigmo_bin(pred, IMAGE_CHANS, IMAGE_WIDTH*IMAGE_HEIGHT);
#endif
    
    char pred_bin_img_fname[256];
    sprintf(pred_bin_img_fname, TRAIN_DIR"test-pred-bin");
    bl_save_image_mc(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANS,
                     pred, pred_bin_img_fname);
#endif
    
    if ((score_filename != NULL) && (strlen(score_filename) > 0))
    {
        FILE *score_file = fopen(score_filename, "ab");
        fprintf(score_file, "%f ", score);
        fclose(score_file);
    }
    
    //
    free(X);
    free(y);
    
    free_images(mix0);
    
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
    
    char *mixfile = option_find_str(options, "train0", "");
    
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
    
    long int num_data = get_num_data_samples(mixfile);
    
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
        //double time0 = what_time_is_it_now();
        
        data train = random_data_samples(mixfile,
                                         stemfile0, stemfile1, stemfile2, stemfile3,
                                         MODE_TRAIN, (int)train_index, net->batch);
        
        train_index += net->batch;
        sample_index += net->batch;
        
        double time0 = what_time_is_it_now();
        
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
        long int num_data_per_epoch = num_data*TRAIN_RATIO*NUM_DATA_EPOCH_RATIO;
        
        //
        double epoch_time0 = what_time_is_it_now();
        double elapsed = epoch_time0 - epoch_time;
        
        if (train_index > 0)
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
            
            _darknet_quiet_flag = 1;
            
            char buff[256];
            sprintf(buff, "%s/%s.backup", backup_directory, base);
            save_weights(net, buff);
            
            char buff2[256];
            sprintf(buff2, "%s/%s_%d.weights", backup_directory, base,
                    (int)epoch_num);
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
            
            _darknet_quiet_flag = 1;
            // WARNING: this makes an undefined behavior, when setting net batch to 1,
            // after having loaded with 20 batches in cfg file
            dump_scores(cfgfile, buff,
                        mixfile,
                        stemfile0, stemfile1, stemfile2, stemfile3,
                        &avg_score_train, &avg_score_test);
            _darknet_quiet_flag = 0;
            
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
        fprintf(stderr,
                "usage: %s %s [train/test/valid] [cfg] [weights (optional)]\n",
                argv[0], argv[1]);
        
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

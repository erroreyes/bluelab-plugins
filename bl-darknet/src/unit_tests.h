#ifndef bl_darknet_unit_tests_h
#define bl_darknet_unit_tests_h

#define FILL_IMG_ALPHA 1

// utils
extern char *gen_fname(const char *fname, int file_num);

extern float *make_image0(int width, int height, int nchan);
// Draw a test pattern inside an image
extern void draw_test_pattern(float *img, int width, int height, int nchan);

extern float *alloc_image_buf(int width, int height, int nchan);

extern void compute_diff(const float *a, const float *b, float *c, int size);

//
extern void run_unit_tests(int argc, char **argv);

#endif

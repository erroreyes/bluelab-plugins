//
//  ImageInpainting.cpp
//  BL-Ghost
//
//  Created by Pan on 09/07/18.
//
//

#include <stdlib.h>

#include <IsophoteInpaint.h>

// Debug
#include <PPMFile.h>

#include <BLTypes.h>

#include "ImageInpaint2.h"

#define MASK_BORDER_SIZE 1

// Improve the inpainting when we use only one of the two directions
// (useful when processing very large heights, for example inpainting
// vertically the highest frequencies)
#define UNIDIRECTION_IMPROV 1

void
ImageInpaint2::Inpaint(BL_FLOAT *image, int width, int height,
                       BL_FLOAT borderRatio,
                       bool processHorizontal, bool processVertical)
{
        
    // Create the mask
    BL_FLOAT *mask = (BL_FLOAT*)malloc(sizeof(BL_FLOAT)*width*height);
    
#if 0 // Use border ratio
      // Problem due to log scale, that is not applied to the border
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            BL_FLOAT col = 0.0;
            
            bool inside = ((i > borderRatio*width) && (i < (1.0 - borderRatio)*width) &&
                           (j > borderRatio*height) && (j < (1.0 - borderRatio)*height));

            if (inside)
                col = 1.0;
            
            mask[i + j*width] = col;
        }
    }
#endif

    //PPMFile::SavePPM("image0.ppm", image, width, height, 1, 24.0*255.0*255.0);
    
    // GOOD !
#if 1 // Keep only a band of 1 pixel around
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            BL_FLOAT col = 0.0;
            
            bool inside = ((i >= MASK_BORDER_SIZE) && (i < width - MASK_BORDER_SIZE) &&
                           (j >= MASK_BORDER_SIZE) && (j < height - MASK_BORDER_SIZE));
            
            if (inside)
                col = 1.0;
            
            mask[i + j*width] = col;
        }
    }
#endif

    //PPMFile::SavePPM("mask.ppm", mask, width, height, 1, 255.0);
    
#if UNIDIRECTION_IMPROV
    // Fill the border with a gradient from the corner points
    // Use 1 pixel border
    
    // If horizontal only
    if (processHorizontal && !processVertical)
    {
        BL_FLOAT corners[2][2];
        corners[0][0] = image[0];
        corners[0][1] = image[width - 1];
        
        corners[1][0] = image[(height - 1)*width];
        corners[1][1] = image[(height - 1)*width + width - 1];
        
        for (int i = 0; i < width; i++)
        {
            BL_FLOAT t = ((BL_FLOAT)i)/(width - 1);
            
            BL_FLOAT col0 = (1.0 - t)*corners[0][0] + t*corners[0][1];
            BL_FLOAT col1 = (1.0 - t)*corners[1][0] + t*corners[1][1];
            
            image[i] = col0;
            image[(height - 1)*width - 1 + i] = col1;
        }
    }
    
    // If vertical only
    if (!processHorizontal && processVertical)
    {
        BL_FLOAT corners[2][2];
        corners[0][0] = image[0];
        corners[0][1] = image[(height - 1)*width];
        
        corners[1][0] = image[width - 1];
        corners[1][1] = image[(height - 1)*width + width - 1];
        
        for (int j = 0; j < height; j++)
        {
            BL_FLOAT t = ((BL_FLOAT)j)/(height - 1);
            
            BL_FLOAT col0 = (1.0 - t)*corners[0][0] + t*corners[0][1];
            BL_FLOAT col1 = (1.0 - t)*corners[1][0] + t*corners[1][1];
            
            image[j*width] = col0;
            image[width - 1 + j*width ] = col1;
        }
    }
#endif
    
    //PPMFile::SavePPM("image1.ppm", image, width, height, 1, 24.0*255.0*255.0);
    
    IsophoteInpaint inpaint(processHorizontal, processVertical);
    
    BL_FLOAT *result;
    inpaint.Process(image, mask, &result, width, height);
    
    if (result != NULL)
    {
        memcpy(image, result, width*height*sizeof(BL_FLOAT));
        free(result);
    }
    
    free(mask);
    
    //PPMFile::SavePPM("image2.ppm", image, width, height, 1, 24.0*255.0*255.0);
}

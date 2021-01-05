#ifdef IGRAPHICS_NANOVG

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <BLBitmap.h>
#include <BLUtils.h>

BLBitmap::BLBitmap(int width, int height, int bpp,
                   unsigned char *data)
{
    mWidth = width;
    mHeight = height;
    mBpp = bpp;
    
    mData = data;
}

BLBitmap::~BLBitmap()
{
    delete mData;
}

int
BLBitmap::GetWidth()
{
    return mWidth;
}

int
BLBitmap::GetHeight()
{
    return mHeight;
}

int
BLBitmap::GetBpp()
{
    return mBpp;
}

unsigned char *
BLBitmap::GetData()
{
    return mData;
}

BLBitmap *
BLBitmap::Load(const char *fileName)
{
    int w, h, n;
    unsigned char *data;
    
    stbi_set_unpremultiply_on_load(1);
    stbi_convert_iphone_png_to_rgb(1);
    data = stbi_load(fileName, &w, &h, &n, 4);
    if (data == NULL)
    {
        //		printf("Failed to load %s - %s\n", filename, stbi_failure_reason());
       
        return NULL;
    }
    
    BLBitmap *bmp = new BLBitmap(w, h, n, data);
    return bmp;
}

#endif

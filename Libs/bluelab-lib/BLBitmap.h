#ifndef BL_BITMAP_H
#define BL_BITMAP_H

#include "IPlug_include_in_plug_hdr.h"

#ifdef IGRAPHICS_NANOVG

class BLBitmap
{
public:
    BLBitmap(int width, int height, int bpp,
             unsigned char *data);

    virtual ~BLBitmap();
    
    int GetWidth();
    int GetHeight();
    int GetBpp();
    
    unsigned char *GetData();
    
    // Load from file
    static BLBitmap *Load(const char *fileName);
  
protected:
    int mWidth;
    int mHeight;
    int mBpp;
    
    unsigned char *mData;
};

#endif

#endif

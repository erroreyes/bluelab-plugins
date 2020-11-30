//
//  SpectrogramDisplay2.cpp
//  BL-Ghost
//
//  Created by Pan on 14/06/18.
//
//

#ifdef IGRAPHICS_NANOVG

#include <BLSpectrogram4.h>
#include <MiniView.h>

#include "SpectrogramDisplay2.h"

#define USE_ZOOM_NVG_TRANSFORM 1

#define USE_SPECTRO_NEAREST 0

SpectrogramDisplay2::SpectrogramDisplay2()
{
    mVg = NULL;
    
    // Spectrogram
    mSpectrogram = NULL;
    mNvgSpectroImage = 0;
    mNeedUpdateSpectrogram = false;
    mNeedUpdateSpectrogramData = false;
    mNvgColormapImage = 0;
    
    mNeedUpdateSpectrogramFullData = false;
    mNvgSpectroFullImage = 0;
    
    mShowSpectrogram = false;
    
    mSpectroCenterPos = 0.5;
    mSpectroAbsTranslation = 0.0;
    
    mSpectroMinX = 0.0;
    mSpectroMaxX = 1.0;
    
    mSpectroAbsMinX = 0.0;
    mSpectroAbsMaxX = 1.0;
    
    mSpectrogramGain = 1.0;
    
    mSpectrogramAlpha = 1.0;
    
    mMiniView = NULL;
    
    mDrawBGSpectrogram = true;
}

SpectrogramDisplay2::~SpectrogramDisplay2()
{
    //if (mMiniView != NULL)
    //    delete mMiniView;
  
    if (mVg == NULL)
      return;
  
    if (mNvgSpectroImage != 0)
        nvgDeleteImage(mVg, mNvgSpectroImage);
    
    if (mNvgColormapImage != 0)
        nvgDeleteImage(mVg, mNvgColormapImage);
    
    if (mNvgSpectroFullImage != 0)
        nvgDeleteImage(mVg, mNvgSpectroFullImage);
}

void
SpectrogramDisplay2::Reset()
{
    mSpectroImageData.Resize(0);
    
    mNeedUpdateSpectrogram = true;   
    mNeedUpdateSpectrogramData = true;
    
    // FIX: with this line uncommented, there is a bug:
    // the background is updated at each edit but with the current zoom
#if 0
    mNeedUpdateSpectrogramFullData = true;
#endif
}

bool
SpectrogramDisplay2::NeedUpdateSpectrogram()
{
    return mNeedUpdateSpectrogram;
}

bool
SpectrogramDisplay2::DoUpdateSpectrogram()
{
    if (mVg == NULL)
      return true;
  
    // Update first, before displaying
    if (!mNeedUpdateSpectrogram)
      //return false;
      // Must return true, because the spectrogram scrolls over time
      // (for smooth scrolling), even if the data is not changed
      return true;
    
    //int w = mSpectrogram->GetMaxNumCols();
    int w = mSpectrogram->GetNumCols();
    int h = mSpectrogram->GetHeight();
    
#if 1 // Avoid white image when there is no data
    if ((w == 0) ||
        (mNvgSpectroImage == 0) || (mNvgSpectroFullImage == 0))
    {
        w = 1;
        int imageSize = w*h*4;
        
        mSpectroImageData.Resize(imageSize);
        memset(mSpectroImageData.Get(), 0, imageSize);
        
        // Spectrogram image
        if (mNvgSpectroImage != 0)
            nvgDeleteImage(mVg, mNvgSpectroImage);
        
        mNvgSpectroImage = nvgCreateImageRGBA(mVg,
                                              w, h,
#if USE_SPECTRO_NEAREST
                                              NVG_IMAGE_NEAREST |
#endif
                                              NVG_IMAGE_ONE_FLOAT_FORMAT,
#endif
                                              mSpectroImageData.Get());
        // Spectrogram full image
        if (mNvgSpectroFullImage != 0)
            nvgDeleteImage(mVg, mNvgSpectroFullImage);
        
        mNvgSpectroFullImage = nvgCreateImageRGBA(mVg,
                                                  w, h,
#if USE_SPECTRO_NEAREST
                                                  NVG_IMAGE_NEAREST |
#endif
                                                  NVG_IMAGE_ONE_FLOAT_FORMAT,
#endif
                                                  mSpectroImageData.Get());
        
        mNeedUpdateSpectrogram = false;
        mNeedUpdateSpectrogramData = false;
        mNeedUpdateSpectrogramFullData = false;
        
        return true;
    }
#endif
    
    int imageSize = w*h*4;
    
    if (mNeedUpdateSpectrogramData ||
        (mNvgSpectroImage == 0) || (mNvgSpectroFullImage == 0))
    {
        if ((mSpectroImageData.GetSize() != imageSize) ||
            (mNvgSpectroImage == 0))
        {
            mSpectroImageData.Resize(imageSize);
            
            memset(mSpectroImageData.Get(), 0, imageSize);
            mSpectrogram->GetImageDataFloat(/*w, h, */mSpectroImageData.Get());
            
            // Spectrogram image
            if (mNvgSpectroImage != 0)
                nvgDeleteImage(mVg, mNvgSpectroImage);
            
            mNvgSpectroImage = nvgCreateImageRGBA(mVg,
                                                  w, h,
#if USE_SPECTRO_NEAREST
                                                  NVG_IMAGE_NEAREST |
#endif
                                                  NVG_IMAGE_ONE_FLOAT_FORMAT,
                                                  mSpectroImageData.Get());
            
			// No need since it has been better fixed in nanovg
#ifdef WIN32 // Hack: after having created the image, update it again
			 // FIX: spectrogram blinking between random pixels and correct pixels  
			//nvgUpdateImage(mVg, mNvgSpectroImage, mSpectroImageData.Get());
#endif

            if (mNeedUpdateSpectrogramFullData || (mNvgSpectroFullImage == 0))
            {
                // Spectrogram full image
                if (mNvgSpectroFullImage != 0)
                    nvgDeleteImage(mVg, mNvgSpectroFullImage);
                
                mNvgSpectroFullImage = nvgCreateImageRGBA(mVg,
                                                          w, h,
#if USE_SPECTRO_NEAREST
                                                          NVG_IMAGE_NEAREST |
#endif
                                                          NVG_IMAGE_ONE_FLOAT_FORMAT,
                                                          mSpectroImageData.Get());

				// No need since it has been better fixed in nanovg
#ifdef WIN32 // Hack: after having created the image, update it again
				// FIX: spectrogram blinking between random pixels and correct pixels  
				//nvgUpdateImage(mVg, mNvgSpectroFullImage, mSpectroImageData.Get());
#endif
            }
        }
        else
        {
            memset(mSpectroImageData.Get(), 0, imageSize);
            bool updated = mSpectrogram->GetImageDataFloat(/*w, h, */mSpectroImageData.Get());

	    if (updated)
	    {
	      // Spectrogram image
	      nvgUpdateImage(mVg, mNvgSpectroImage, mSpectroImageData.Get());
            
	      // Spectrogram full image
	      if (mNeedUpdateSpectrogramFullData)
	      {
		  nvgUpdateImage(mVg, mNvgSpectroFullImage, mSpectroImageData.Get());
	      }
	    }
        }
    }

    if (mNeedUpdateColormapData || (mNvgColormapImage == 0))
    {
      // Colormap
      WDL_TypedBuf<unsigned int> colorMapData;
      mSpectrogram->GetColormapImageDataRGBA(&colorMapData);
    
      if ((colorMapData.GetSize() != mColormapImageData.GetSize()) ||
	  (mNvgColormapImage == 0))
      {
	mColormapImageData = colorMapData;
        
	if (mNvgColormapImage != 0)
	  nvgDeleteImage(mVg, mNvgColormapImage);
        
	mNvgColormapImage = nvgCreateImageRGBA(mVg,
					       mColormapImageData.GetSize(), 1, NVG_IMAGE_NEAREST /*0*/,
					       (unsigned char *)mColormapImageData.Get());
      }
      else
      {
        mColormapImageData = colorMapData;
        
        nvgUpdateImage(mVg, mNvgColormapImage, (unsigned char *)mColormapImageData.Get());
      }
    }
    
    mNeedUpdateSpectrogram = false;
    mNeedUpdateSpectrogramData = false;
    mNeedUpdateSpectrogramFullData = false;
    
    return true;
}

void
SpectrogramDisplay2::PreDraw(NVGcontext *vg, int width, int height)
{
    mVg = vg;

    // Just in case
    DoUpdateSpectrogram();
    
    if (!mShowSpectrogram)
    {
        if (mMiniView != NULL)
        {
            mMiniView->Display(mVg, width, height);
        }
    
        return;
    }
    
    // Draw spectrogram first
    nvgSave(mVg);
    
    // New: set colormap only in the spectrogram state
    nvgSetColormap(mVg, mNvgColormapImage);
    
    //int width = this->mRECT.W();
    //int height = this->mRECT.H();
    
#define DEBUG_DISPLAY_FG 1
#define DEBUG_DISPLAY_BG 1
    
    //nvgSave(mVg);
    
#if DEBUG_DISPLAY_BG
    if (mDrawBGSpectrogram)
    {
        //
        // Spectrogram full image
        //
        // Display the full image first, so it will be in background
    
        //NVGpaint fullImgPaint = nvgImagePattern(mVg, 0.0, 0.0, width, height,
        //                                        0.0, mNvgSpectroFullImage, mSpectrogramAlpha);
        NVGpaint fullImgPaint = nvgImagePattern(mVg,
                                                mSpectrogramBounds[0]*width,
                                                mSpectrogramBounds[1]*height,
                                                (mSpectrogramBounds[2] - mSpectrogramBounds[0])*width,
                                                (mSpectrogramBounds[3] - mSpectrogramBounds[1])*height,
                                                0.0, mNvgSpectroFullImage, mSpectrogramAlpha);
#if USE_ZOOM_NVG_TRANSFORM
        nvgResetTransform(mVg);
    
        BL_FLOAT absZoom = mSpectroAbsMaxX - mSpectroAbsMinX;
    
        nvgTranslate(mVg, mSpectroAbsMinX*width, 0.0);
        nvgScale(mVg, absZoom, 1.0);
#endif
    
        BL_GUI_FLOAT b1f = mSpectrogramBounds[1]*height;
        BL_GUI_FLOAT b3f = (mSpectrogramBounds[3] - mSpectrogramBounds[1])*height;
	// TEST
	//#if GRAPH_CONTROL_FLIP_Y
        //b1f = height - b1f;
        //b3f = height - b3f;
	//#endif
        
        nvgBeginPath(mVg);
    
        // Corner (x, y) => bottom-left
        nvgRect(mVg,
                mSpectrogramBounds[0]*width, b1f,
                (mSpectrogramBounds[2] - mSpectrogramBounds[0])*width, b3f);
    
        nvgFillPaint(mVg, fullImgPaint);
        nvgFill(mVg);
    
#if USE_ZOOM_NVG_TRANSFORM
        nvgResetTransform(mVg); //
#endif
    }
#endif
    
#if DEBUG_DISPLAY_FG
    //
    // Spectrogram image
    //
    
    // Display the rightmost par in case of zoom
    //NVGpaint imgPaint = nvgImagePattern(mVg, 0.0, 0.0, width, height,
    //                                    0.0, mNvgSpectroImage, mSpectrogramAlpha);
    
    NVGpaint imgPaint = nvgImagePattern(mVg,
                                        mSpectrogramBounds[0]*width,
                                        mSpectrogramBounds[1]*height,
                                        (mSpectrogramBounds[2] - mSpectrogramBounds[0])*width,
                                        (mSpectrogramBounds[3] - mSpectrogramBounds[1])*height,
                                        0.0, mNvgSpectroImage, mSpectrogramAlpha);
    
    
#if USE_ZOOM_NVG_TRANSFORM
    nvgResetTransform(mVg);
    
    BL_FLOAT zoom = mSpectroMaxX - mSpectroMinX;
    
    nvgTranslate(mVg, mSpectroMinX*width, 0.0);
    nvgScale(mVg, zoom, 1.0);
#endif
    
    BL_GUI_FLOAT b1f = mSpectrogramBounds[1]*height;
    BL_GUI_FLOAT b3f = (mSpectrogramBounds[3] - mSpectrogramBounds[1])*height;
    // TEST
    //#if GRAPH_CONTROL_FLIP_Y
    //b1f = height - b1f;
    //b3f = height - b3f;
    //#endif
    
    nvgBeginPath(mVg);
    nvgRect(mVg,
            mSpectrogramBounds[0]*width, b1f,
            (mSpectrogramBounds[2] - mSpectrogramBounds[0])*width, b3f);
    
    
    nvgFillPaint(mVg, imgPaint);
    nvgFill(mVg);
    
#if USE_ZOOM_NVG_TRANSFORM
    nvgResetTransform(mVg); //
#endif
    
#endif
    
    nvgRestore(mVg);

    if (mMiniView != NULL)
    {
        mMiniView->Display(mVg, width, height);
    }
}

bool
SpectrogramDisplay2::PointInsideSpectrogram(int x, int y, int width, int height)
{
    // Warning: y is reversed !
    BL_FLOAT nx = ((BL_FLOAT)x)/width;
    BL_FLOAT ny = 1.0 - ((BL_FLOAT)y)/height;
    
    if (nx < mSpectrogramBounds[0])
        return false;
    
    if (ny < mSpectrogramBounds[1])
        return false;
    
    if (nx > mSpectrogramBounds[2])
        return false;
    
    if (ny > mSpectrogramBounds[3])
        return false;
    
    return true;
}

bool
SpectrogramDisplay2::PointInsideMiniView(int x, int y, int width, int height)
{
    if (mMiniView == NULL)
        return false;
    
    bool inside = mMiniView->IsPointInside(x, y, width, height);
    
    return inside;
}

void
SpectrogramDisplay2::GetSpectroNormCoordinate(int x, int y, int width, int height,
                                             BL_FLOAT *nx, BL_FLOAT *ny)
{
    BL_FLOAT nx0 = ((BL_FLOAT)x)/width;
    BL_FLOAT ny0 = ((BL_FLOAT)y)/height;
    
    *nx = (nx0 - mSpectrogramBounds[0])/
            (mSpectrogramBounds[2] - mSpectrogramBounds[0]);
    
    *ny = (ny0 - mSpectrogramBounds[1])/
    (mSpectrogramBounds[3] - mSpectrogramBounds[1]);
}

void
SpectrogramDisplay2::SetBounds(BL_FLOAT left, BL_FLOAT top,
                               BL_FLOAT right, BL_FLOAT bottom)
{
    mSpectrogramBounds[0] = left;
    mSpectrogramBounds[1] = top;
    mSpectrogramBounds[2] = right;
    mSpectrogramBounds[3] = bottom;
}

void
SpectrogramDisplay2::SetSpectrogram(BLSpectrogram4 *spectro)
{
    mSpectrogram = spectro;
    
    mShowSpectrogram = true;
    
    // Be sure to create the texture image in the right thread
    UpdateSpectrogram(true, true);
}

void
SpectrogramDisplay2::SetMiniView(MiniView *view)
{
  mMiniView = view;
}

void
SpectrogramDisplay2::ShowSpectrogram(bool flag)
{
    mShowSpectrogram = flag;
}

void
SpectrogramDisplay2::UpdateSpectrogram(bool updateData, bool updateFullData)
{
    mNeedUpdateSpectrogram = true;
    
    if (!mNeedUpdateSpectrogramData)
        mNeedUpdateSpectrogramData = updateData;
    
    if (!mNeedUpdateSpectrogramFullData)
    {
        if (updateFullData) // For debugging
            mNeedUpdateSpectrogramFullData = updateFullData;
    }
}

void
SpectrogramDisplay2::UpdateColormap(bool flag)
{
    mNeedUpdateSpectrogram = true;
    
    if (!mNeedUpdateColormapData)
    {
        mNeedUpdateColormapData = flag;
    }
}

void
SpectrogramDisplay2::ResetSpectrogramTransform()
{
    mSpectroMinX = 0.0;
    mSpectroMaxX = 1.0;
    
    mSpectroAbsMinX = 0.0;
    mSpectroAbsMaxX = 1.0;
    
    mSpectroCenterPos = 0.5;
    mSpectroAbsTranslation = 0.0;
}

void
SpectrogramDisplay2::ResetSpectrogramTranslation()
{
    mSpectroAbsTranslation = 0.0;
}

void
SpectrogramDisplay2::SetSpectrogramZoom(BL_FLOAT zoomX)
{
    BL_FLOAT norm = (mSpectroCenterPos - mSpectroMinX)/(mSpectroMaxX - mSpectroMinX);
    
    mSpectroMinX = mSpectroCenterPos - norm*zoomX;
    mSpectroMaxX = mSpectroCenterPos + (1.0 - norm)*zoomX;
}

void
SpectrogramDisplay2::SetSpectrogramAbsZoom(BL_FLOAT zoomX)
{
    BL_FLOAT norm = (mSpectroCenterPos - mSpectroAbsMinX)/(mSpectroAbsMaxX - mSpectroAbsMinX);
    
    mSpectroAbsMinX = mSpectroCenterPos - norm*zoomX;
    mSpectroAbsMaxX = mSpectroCenterPos + (1.0 - norm)*zoomX;
}

void
SpectrogramDisplay2::SetSpectrogramCenterPos(BL_FLOAT centerPos)
{
    mSpectroCenterPos = centerPos;
}

bool
SpectrogramDisplay2::SetSpectrogramTranslation(BL_FLOAT tX)
{
    BL_FLOAT dX = tX - mSpectroAbsTranslation;
    
    if (((mSpectroAbsMinX > 1.0) && (dX > 0.0)) ||
        ((mSpectroAbsMaxX < 0.0) && (dX < 0.0)))
        // Do not translate if the spectrogram gets out of view
        return false;
    
    mSpectroAbsTranslation = tX;
    
    mSpectroMinX += dX;
    mSpectroMaxX += dX;
    
    mSpectroAbsMinX += dX;
    mSpectroAbsMaxX += dX;
    
    return true;
}

void
SpectrogramDisplay2::GetSpectrogramVisibleNormBounds(BL_FLOAT *minX, BL_FLOAT *maxX)
{
    if (mSpectroAbsMinX < 0.0)
        *minX = -mSpectroAbsMinX/(mSpectroAbsMaxX - mSpectroAbsMinX);
    else
        *minX = 0.0;
    
    if (mSpectroAbsMaxX < 1.0)
        *maxX = 1.0;
    else
        *maxX = 1.0 - (mSpectroAbsMaxX - 1.0)/(mSpectroAbsMaxX - mSpectroAbsMinX);
}

void
SpectrogramDisplay2::GetSpectrogramVisibleNormBounds2(BL_FLOAT *minX, BL_FLOAT *maxX)
{
    *minX = -mSpectroAbsMinX/(mSpectroAbsMaxX - mSpectroAbsMinX);
    *maxX = 1.0 - (mSpectroAbsMaxX - 1.0)/(mSpectroAbsMaxX - mSpectroAbsMinX);
}

void
SpectrogramDisplay2::SetSpectrogramVisibleNormBounds2(BL_FLOAT minX, BL_FLOAT maxX)
{
    minX *= mSpectroAbsMaxX - mSpectroAbsMinX;
    BL_FLOAT spectroAbsMinX = -minX;
    
    //maxX -= 1.0;
    maxX *= mSpectroAbsMaxX - mSpectroAbsMinX;
    //maxX -= 1.0;
    
    BL_FLOAT spectroAbsMaxX = -maxX;
    
    BL_FLOAT trans = spectroAbsMinX - mSpectroAbsMinX;
    
    mSpectroAbsMinX = spectroAbsMinX;
    mSpectroAbsMaxX = spectroAbsMaxX;
    
    //
    mSpectroMinX += trans;
    mSpectroMaxX += trans;
    
    mSpectroCenterPos += trans;
}

void
SpectrogramDisplay2::ResetSpectrogramZoomAndTrans()
{
    mSpectroMinX = 0.0;
    mSpectroMaxX = 1.0;
}

MiniView *
SpectrogramDisplay2::GetMiniView()
{
    return mMiniView;
}

void
SpectrogramDisplay2::SetDrawBGSpectrogram(bool flag)
{
    mDrawBGSpectrogram = flag;
}

#endif // IGRAPHICS_NANOVG

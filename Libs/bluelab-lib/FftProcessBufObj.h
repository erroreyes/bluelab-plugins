/* Copyright (C) 2022 Nicolas Dittlo <deadlab.plugins@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#ifndef FFT_PROCESS_BUF_OBJ_H
#define FFT_PROCESS_BUF_OBJ_H

#include <FftProcessObj16.h>

#include "IGraphics_include_in_plug_hdr.h"

class FftProcessBufObj : public ProcessObj
{
public:
  FftProcessBufObj(int bufferSize, int oversampling,
                   int freqRes, BL_FLOAT sampleRate);
  
  virtual ~FftProcessBufObj();
  
  void ProcessFftBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer,
                        const WDL_TypedBuf<WDL_FFT_COMPLEX> *scBuffer = NULL);
  
  void GetComplexBuffer(WDL_TypedBuf<WDL_FFT_COMPLEX> *ioBuffer);
  
  void GetBuffer(WDL_TypedBuf<BL_FLOAT> *ioBuffer);
  
private:
    WDL_TypedBuf<WDL_FFT_COMPLEX> mCurrentBuf;

    bool mMagnsBufValid;
    WDL_TypedBuf<BL_FLOAT> mCurrentMagnsBuf;
};

#endif

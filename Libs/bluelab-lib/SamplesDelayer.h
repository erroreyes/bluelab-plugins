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
 
//
//  SamplesDelayer.h
//  Transient
//
//  Created by Apple m'a Tuer on 24/05/17.
//
//

#ifndef __Transient__SamplesDelayer__
#define __Transient__SamplesDelayer__

#include <BLTypes.h>

#include "IPlug_include_in_plug_hdr.h"
//#include "../../WDL/IPlug/Containers.h"

// Delays output depending on input, by nFrames
class SamplesDelayer
{
public:
    SamplesDelayer(int nFrames);
    
    virtual ~SamplesDelayer();
    
    // Return true if output is available
    bool Process(const BL_FLOAT *input, BL_FLOAT *output, int nFrames);
    
    void Reset();
    
protected:
    int mNFrames;
    
    WDL_TypedBuf<BL_FLOAT> mSamples;
};

#endif /* defined(__Transient__SamplesDelayer__) */

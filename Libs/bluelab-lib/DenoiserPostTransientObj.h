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
//  DenoiserPostTransientObj.h
//  BL-Denoiser
//
//  Created by applematuer on 6/25/20.
//
//

#ifndef __BL_Denoiser__DenoiserPostTransientObj__
#define __BL_Denoiser__DenoiserPostTransientObj__

#include <PostTransientFftObj3.h>

// DenoiserPostTransientObj
// Use only 2 TransientShaperFftObj3 instead of 4
// We use one for left channel, and one for right channel.
// We use the same for signal L and noise L.
// We use the same for signal R and noise R.
class DenoiserPostTransientObj : public PostTransientFftObj3
{
public:
    DenoiserPostTransientObj(const vector<ProcessObj *> &processObjs,
                             int numChannels, int numScInputs,
                             int bufferSize, int overlapping, int freqRes,
                             BL_FLOAT sampleRate,
                             BL_FLOAT freqAmpRatio = -1.0,
                             BL_FLOAT transBoostFactor = -1.0);
    
    virtual ~DenoiserPostTransientObj();
    
protected:
    void ResultSamplesWinReady() override;
    
    // Override FftProcessObj16
    void ProcessAllFftSteps() override;
};

#endif /* defined(__BL_Denoiser__DenoiserPostTransientObj__) */

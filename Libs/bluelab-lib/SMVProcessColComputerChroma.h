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
//  SMVProcessYComputerChroma.h
//  BL-SoundMetaViewer
//
//  Created by applematuer on 3/24/20.
//
//

#ifndef __BL_SoundMetaViewer__SMVProcessColComputerChroma__
#define __BL_SoundMetaViewer__SMVProcessColComputerChroma__

#include <SMVProcessColComputer.h>

//
class SMVProcessColComputerChroma : public SMVProcessColComputer
{
public:
    SMVProcessColComputerChroma(BL_FLOAT sampleRate);
    
    virtual ~SMVProcessColComputerChroma();
    
    virtual void Reset(BL_FLOAT sampleRate) override;
    
    void ComputeCol(const WDL_TypedBuf<BL_FLOAT> magns[2],
                    const WDL_TypedBuf<BL_FLOAT> phases[2],
                    const WDL_TypedBuf<BL_FLOAT> phasesUnwrap[2],
                    WDL_TypedBuf<BL_FLOAT> *resultCol) override;
    
protected:
    BL_FLOAT mSampleRate;
};

#endif /* defined(__BL_SoundMetaViewer__SMVProcessYComputerChroma__) */

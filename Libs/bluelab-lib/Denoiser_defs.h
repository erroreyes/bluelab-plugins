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
//  Denoiser_defs.h
//  BL-Denoiser
//
//  Created by applematuer on 6/25/20.
//
//

#ifndef BL_Denoiser_Denoiser_defs_h
#define BL_Denoiser_Denoiser_defs_h

// Set to 1, in addition, it seems to fix a bug:
//
// Learn at 44100Hz (make a peak at 500Hz with BL-Sine
// Then switch to 88200 => the peak is no more at 500Hz
#define USE_VARIABLE_BUFFER_SIZE 1

#define USE_AUTO_RES_NOISE 1

#define USE_RESIDUAL_DENOISE 1

//#define MIN_DB -120.0
//#define MAX_DB 0.0

#define DENOISER_MIN_DB -119.0 // Take care of the noise/harmo bottom dB
#define DENOISER_MAX_DB 10.0

#endif

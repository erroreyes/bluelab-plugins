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
//  ImageInpaint.h
//  BL-Ghost
//
//  Created by Pan on 09/07/18.
//
//

#ifndef __BL_Ghost__ImageInpaint__
#define __BL_Ghost__ImageInpaint__

// Uses "gti" (gaussian)
// Works when the background to keep is a regular texture
// (such as noise, or regular background)
//
// Not desinged to manage shapes
//
class ImageInpaint
{
public:
    static void Inpaint(BL_FLOAT *image, int width, int height,
                        BL_FLOAT borderRatio);
};

#endif /* defined(__BL_Ghost__ImageInpaint__) */

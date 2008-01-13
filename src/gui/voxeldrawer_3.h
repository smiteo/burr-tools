/* Burr Solver
 * Copyright (C) 2003-2008  Andreas Röver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef __VOXEL_DRAWER_3_H__
#define __VOXEL_DRAWER_3_H__

#include "voxeldrawer.h"

class gridType_c;

class voxelDrawer_3_c : public voxelDrawer_c {

  public:

    voxelDrawer_3_c(void) {}

    void drawFrame(const voxel_c * space, int x, int y, int z, float edge);
    void drawNormalVoxel(const voxel_c * space, int x, int y, int z, float alpha, float edge);
    void drawVariableMarkers(const voxel_c * space, int x, int y, int z);
    void drawCursor(const voxel_c * space, int mX1, int mX2, int mY1, int mY2, int mZ, int mode);

    void calculateSize(const voxel_c * shape, float * x, float * y, float * z);
    void recalcSpaceCoordinates(float * x, float * y, float * z);
};

#endif
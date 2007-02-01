/* Burr Solver
 * Copyright (C) 2003-2006  Andreas R�ver
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
#ifndef __GRID_EDITOR_2_H__
#define __GRID_EDITOR_2_H__

#include "grideditor.h"

class puzzle_c;

/**
 * this widget allows to edit voxel spaces. It shows one Z-Layer of the space as a grid
 * of spheres and each of these squares can be toggled between the state filled, variable, empty
 *
 * the callback is issued on on the following occasions:
 *  - Mouse moves inside the area of the widget (for the 3d view to show the cursor
 *  - Changes of voxel states
 *
 *  this is by faaaaar the most ugly code in the whole project, this really needs a clean rewrite
 */
class gridEditor_2_c : public gridEditor_c {

private:

  // calculate the size of the grid cells and the
  // top right corner position
  void calcParameters(int *sx, int *sy, int *tx, int *ty);

  void calcGridPosition(int x, int y, int z, int *gx, int *gy);

  void drawNormalTile(int x, int y, int z, int tx, int ty, int sx, int sy);
  void drawVariableTile(int x, int y, int z, int tx, int ty, int sx, int sy);
  void drawTileFrame(int x, int y, int z, int tx, int ty, int sx, int sy);
  void drawTileColor(int x, int y, int z, int tx, int ty, int sx, int sy);
  void drawTileCursor(int x, int y, int z, int x1, int y1, int x2, int y2, int tx, int ty, int sx, int sy);
  bool validCoordinates(int x, int y, int z);

public:

  gridEditor_2_c(int x, int y, int w, int h, puzzle_c * p) : gridEditor_c(x, y, w, h, p) {}
};

#endif

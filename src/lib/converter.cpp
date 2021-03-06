/* BurrTools
 *
 * BurrTools is the legal property of its developers, whose
 * names are listed in the COPYRIGHT file, which is included
 * within the source distribution.
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
#include "converter.h"

#include "puzzle.h"
#include "problem.h"
#include "voxel.h"

bool canConvert(gridType_c::gridType src, gridType_c::gridType dst)
{
  return (
          (src == gridType_c::GT_BRICKS && dst == gridType_c::GT_RHOMBIC)
          ||
          (src == gridType_c::GT_BRICKS && dst == gridType_c::GT_TETRA_OCTA)
         );
}

/* do the conversion, if it can't be done (you should check first)
 * nothing will happen.
 * the shapes inside the puzzle will be converted and then the
 * gridtype within the puzzle will be changed
 */
puzzle_c * doConvert(puzzle_c * p, gridType_c::gridType type) {

  if (!canConvert(p->getGridType()->getType(), type)) return 0;

  // for now only the conversion from brick to rhombic is available

  // create a gridType for the target grid
  gridType_c * gt = new gridType_c(type);

  puzzle_c * pNew = new puzzle_c(gt);

  // now convert all shapes
  for (unsigned int i = 0; i < p->getNumberOfShapes(); i++)
  {
    const voxel_c * v = p->getShape(i);

    voxel_c * vn = 0;
    // this is now conversion specific....

    if (p->getGridType()->getType() == gridType_c::GT_BRICKS && type == gridType_c::GT_RHOMBIC)
    {
      vn = gt->getVoxel(v->getX()*5, v->getY()*5, v->getZ()*5, voxel_c::VX_EMPTY);
      vn->skipRecalcBoundingBox(true);

      for (unsigned int x = 0; x < v->getX(); x++)
        for (unsigned int y = 0; y < v->getY(); y++)
          for (unsigned int z = 0; z < v->getZ(); z++) {
            voxel_type st = v->get(x, y, z);

            if (st != 0)
              for (int ax = 0; ax < 5; ax++)
                for (int ay = 0; ay < 5; ay++)
                  for (int az = 0; az < 5; az++)
                    if (vn->validCoordinate(5*x+ax, 5*y+ay, 5*z+az))
                      vn->set(5*x+ax, 5*y+ay, 5*z+az, st);
          }
    }
    else if (p->getGridType()->getType() == gridType_c::GT_BRICKS && type == gridType_c::GT_TETRA_OCTA)
    {
      vn = gt->getVoxel(v->getX()*3, v->getY()*3, v->getZ()*3, voxel_c::VX_EMPTY);
      vn->skipRecalcBoundingBox(true);

      for (unsigned int x = 0; x < v->getX(); x++)
        for (unsigned int y = 0; y < v->getY(); y++)
          for (unsigned int z = 0; z < v->getZ(); z++) {
            voxel_type st = v->get(x, y, z);

            if (st != 0)
              for (int ax = 0; ax < 3; ax++)
                for (int ay = 0; ay < 3; ay++)
                  for (int az = 0; az < 3; az++)
                    if (vn->validCoordinate(3*x+ax, 3*y+ay, 3*z+az))
                      vn->set(3*x+ax, 3*y+ay, 3*z+az, st);
          }
    }
    else
      bt_assert(0);

    vn->skipRecalcBoundingBox(false);
    vn->setName(v->getName());
    pNew->addShape(vn);
  }

  // copy over colours
  for (unsigned int i = 0; i < p->colorNumber(); i++)
  {
    unsigned char r, g, b;
    p->getColor(i, &r, &g, &b);
    pNew->addColor(r, g, b);
  }

  // copy comment
  pNew->setComment(p->getComment());
  pNew->setCommentPopup(p->getCommentPopup());

  // create the problems as copies from the old puzzle
  for (unsigned int i = 0; i < p->getNumberOfProblems(); i++)
  {
    const problem_c * prob = p->getProblem(i);
    problem_c * probNew = pNew->getProblem(pNew->addProblem(prob));

    probNew->setName(prob->getName());
  }

  return pNew;
}


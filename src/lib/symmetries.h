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
#ifndef __SYMMETRIES_H__
#define __SYMMETRIES_H__

class gridType_c;

// this modules contains just some helper functions for transformations and symmetrie handling

/* this type is used to collect all the symmetries that a piece can have. For each symmetry
 * the corresponding bit is set
 */
typedef unsigned char symmetries_t;

/* some macros for ht esymmeties_t type
 */

/* symmetry 0 always correspondy to the completely unsymmetric case
 */
#define unSymmetric(s) ((s) == 0)

/* the higest symmetry number is 0xff and that is the invaid case. Let's hope that there is
 * no grid space that has more than 255 different symmetries
 */
#define symmetryInvalid() (0xFF)
#define isSymmetryInvalid(s) ((s) == 0xFF)

class voxel_c;

class symmetries_c {

  public:

    virtual unsigned int getNumTransformations(void) const = 0;
    virtual unsigned int getNumTransformationsMirror(void) const = 0;

    /* one piece can have 48 symmetries. 24 rotational and another 24 rotational with mirroring.
     * the mirroring is possible to avoid finding mirrored solutions.
     * All the symmetries are enumbered. The first 24 are the not mirrored. the other 24 are
     * first mirrored along the x axins (-x -> x). The the piece is rotated around the x axis by
     * the value contained inside this array at the position of the symmetry number or symmetry number
     * minus 24. The the piece is rotated around y and finally around z.
     * If the resulting piece is identical to the original untransformed than the piece has the
     * symmetry or the given rotation number
     */
    virtual int rotx(unsigned int p) const = 0;
    virtual int roty(unsigned int p) const = 0;
    virtual int rotz(unsigned int p) const = 0;

    /* this return true, if the symmetry contains the given transformation */
    virtual bool symmetrieContainsTransformation(symmetries_t s, unsigned int t) const = 0;

    /* returns the transformation that results, whenyou first carry out transformation t1
     * and then transformation t2
     */
    virtual unsigned char transAdd(unsigned char t1, unsigned char t2) const = 0;

    /* find the first transformation, for a shape with the given transformation that
     * retults in a shape identical to the given transformation
     */
    virtual unsigned char minimizeTransformation(symmetries_t s, unsigned char trans) const = 0;

    /* this counts how many 'overlap' there is in symmetrie between the given result and the shape
     * the value is smalle, when the 2 shapes have less symmetries in common
     */
    virtual unsigned int countSymmetryIntersection(symmetries_t resultSym, symmetries_t s2) const = 0;


    virtual bool symmetriesLeft(symmetries_t resultSym, symmetries_t s2) const = 0;

    /* calculate the symmetry mask for the given voxel space */
    virtual symmetries_t symmetryCalcuation(const voxel_c * pp) const = 0;
};

/* this is the symmetries class for cubes */
class symmetries_0_c : public symmetries_c {

  /* this varable contains the symmetries that are available because
   * of the subsetting of the cube because of skewing and scaling along the different axes
   * this is used to suppres certain symmetries for the placement and symmetry checks
   *
   * this value infuences the rotx y z functions to only return those transformations
   * that are really available, als getNumTransformations and getNumTransformationsMirror are
   * influenced to only return the number of symmetries within this field, ...
   */
  symmetries_t subSymmetry;

  public:

    symmetries_0_c(const gridType_c * gt);

    unsigned int getNumTransformations(void) const;
    unsigned int getNumTransformationsMirror(void) const;
    int rotx(unsigned int p) const;
    int roty(unsigned int p) const;
    int rotz(unsigned int p) const;
    bool symmetrieContainsTransformation(symmetries_t s, unsigned int t) const;
    unsigned char transAdd(unsigned char t1, unsigned char t2) const;
    unsigned char minimizeTransformation(symmetries_t s, unsigned char trans) const;
    unsigned int countSymmetryIntersection(symmetries_t resultSym, symmetries_t s2) const;
    bool symmetriesLeft(symmetries_t resultSym, symmetries_t s2) const;
    symmetries_t symmetryCalcuation(const voxel_c * pp) const;
};

#endif

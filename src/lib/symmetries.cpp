/* Burr Solver
 * Copyright (C) 2003-2005  Andreas R�ver
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


#include "symmetries.h"

#include <assert.h>

static int _rotx[24] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3 };
static int _roty[24] = { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 };
static int _rotz[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 3, 3 };

int rotx(unsigned int p) {
  assert(p < 24);
  return _rotx[p];
}
int roty(unsigned int p) {
  assert(p < 24);
  return _roty[p];
}
int rotz(unsigned int p) {
  assert(p < 24);
  return _rotz[p];
}

unsigned int transMult[48][48] = {
  { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
  { 1,  2,  3,  0,  5,  6,  7,  4,  9, 10, 11,  8, 13, 14, 15, 12, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24, 29, 30, 31, 28, 33, 34, 35, 32, 37, 38, 39, 36, 41, 42, 43, 40, 45, 46, 47, 44},
  { 2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13, 18, 19, 16, 17, 22, 23, 20, 21, 26, 27, 24, 25, 30, 31, 28, 29, 34, 35, 32, 33, 38, 39, 36, 37, 42, 43, 40, 41, 46, 47, 44, 45},
  { 3,  0,  1,  2,  7,  4,  5,  6, 11,  8,  9, 10, 15, 12, 13, 14, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26, 31, 28, 29, 30, 35, 32, 33, 34, 39, 36, 37, 38, 43, 40, 41, 42, 47, 44, 45, 46},
  { 4, 21, 14, 19,  8, 22,  2, 18, 12, 23,  6, 17,  0, 20, 10, 16,  5,  1, 13,  9,  7, 11, 15,  3, 36, 41, 30, 47, 24, 40, 34, 44, 28, 43, 38, 45, 32, 42, 26, 46, 39, 35, 31, 27, 37, 25, 29, 33},
  { 5, 22, 15, 16,  9, 23,  3, 19, 13, 20,  7, 18,  1, 21, 11, 17,  6,  2, 14, 10,  4,  8, 12,  0, 37, 42, 31, 44, 25, 41, 35, 45, 29, 40, 39, 46, 33, 43, 27, 47, 36, 32, 28, 24, 38, 26, 30, 34},
  { 6, 23, 12, 17, 10, 20,  0, 16, 14, 21,  4, 19,  2, 22,  8, 18,  7,  3, 15, 11,  5,  9, 13,  1, 38, 43, 28, 45, 26, 42, 32, 46, 30, 41, 36, 47, 34, 40, 24, 44, 37, 33, 29, 25, 39, 27, 31, 35},
  { 7, 20, 13, 18, 11, 21,  1, 17, 15, 22,  5, 16,  3, 23,  9, 19,  4,  0, 12,  8,  6, 10, 14,  2, 39, 40, 29, 46, 27, 43, 33, 47, 31, 42, 37, 44, 35, 41, 25, 45, 38, 34, 30, 26, 36, 24, 28, 32},
  { 8, 11, 10,  9, 12, 15, 14, 13,  0,  3,  2,  1,  4,  7,  6,  5, 22, 21, 20, 23, 18, 17, 16, 19, 32, 35, 34, 33, 36, 39, 38, 37, 24, 27, 26, 25, 28, 31, 30, 29, 46, 45, 44, 47, 42, 41, 40, 43},
  { 9,  8, 11, 10, 13, 12, 15, 14,  1,  0,  3,  2,  5,  4,  7,  6, 23, 22, 21, 20, 19, 18, 17, 16, 33, 32, 35, 34, 37, 36, 39, 38, 25, 24, 27, 26, 29, 28, 31, 30, 47, 46, 45, 44, 43, 42, 41, 40},
  {10,  9,  8, 11, 14, 13, 12, 15,  2,  1,  0,  3,  6,  5,  4,  7, 20, 23, 22, 21, 16, 19, 18, 17, 34, 33, 32, 35, 38, 37, 36, 39, 26, 25, 24, 27, 30, 29, 28, 31, 44, 47, 46, 45, 40, 43, 42, 41},
  {11, 10,  9,  8, 15, 14, 13, 12,  3,  2,  1,  0,  7,  6,  5,  4, 21, 20, 23, 22, 17, 16, 19, 18, 35, 34, 33, 32, 39, 38, 37, 36, 27, 26, 25, 24, 31, 30, 29, 28, 45, 44, 47, 46, 41, 40, 43, 42},
  {12, 17,  6, 23,  0, 16, 10, 20,  4, 19, 14, 21,  8, 18,  2, 22, 15, 11,  7,  3, 13,  1,  5,  9, 28, 45, 38, 43, 32, 46, 26, 42, 36, 47, 30, 41, 24, 44, 34, 40, 29, 25, 37, 33, 31, 35, 39, 27},
  {13, 18,  7, 20,  1, 17, 11, 21,  5, 16, 15, 22,  9, 19,  3, 23, 12,  8,  4,  0, 14,  2,  6, 10, 29, 46, 39, 40, 33, 47, 27, 43, 37, 44, 31, 42, 25, 45, 35, 41, 30, 26, 38, 34, 28, 32, 36, 24},
  {14, 19,  4, 21,  2, 18,  8, 22,  6, 17, 12, 23, 10, 16,  0, 20, 13,  9,  5,  1, 15,  3,  7, 11, 30, 47, 36, 41, 34, 44, 24, 40, 38, 45, 28, 43, 26, 46, 32, 42, 31, 27, 39, 35, 29, 33, 37, 25},
  {15, 16,  5, 22,  3, 19,  9, 23,  7, 18, 13, 20, 11, 17,  1, 21, 14, 10,  6,  2, 12,  0,  4,  8, 31, 44, 37, 42, 35, 45, 25, 41, 39, 46, 29, 40, 27, 47, 33, 43, 28, 24, 36, 32, 30, 34, 38, 26},
  {16,  5, 22, 15, 19,  9, 23,  3, 18, 13, 20,  7, 17,  1, 21, 11, 10,  6,  2, 14,  0,  4,  8, 12, 44, 37, 42, 31, 45, 25, 41, 35, 46, 29, 40, 39, 47, 33, 43, 27, 24, 36, 32, 28, 34, 38, 26, 30},
  {17,  6, 23, 12, 16, 10, 20,  0, 19, 14, 21,  4, 18,  2, 22,  8, 11,  7,  3, 15,  1,  5,  9, 13, 45, 38, 43, 28, 46, 26, 42, 32, 47, 30, 41, 36, 44, 34, 40, 24, 25, 37, 33, 29, 35, 39, 27, 31},
  {18,  7, 20, 13, 17, 11, 21,  1, 16, 15, 22,  5, 19,  3, 23,  9,  8,  4,  0, 12,  2,  6, 10, 14, 46, 39, 40, 29, 47, 27, 43, 33, 44, 31, 42, 37, 45, 35, 41, 25, 26, 38, 34, 30, 32, 36, 24, 28},
  {19,  4, 21, 14, 18,  8, 22,  2, 17, 12, 23,  6, 16,  0, 20, 10,  9,  5,  1, 13,  3,  7, 11, 15, 47, 36, 41, 30, 44, 24, 40, 34, 45, 28, 43, 38, 46, 32, 42, 26, 27, 39, 35, 31, 33, 37, 25, 29},
  {20, 13, 18,  7, 21,  1, 17, 11, 22,  5, 16, 15, 23,  9, 19,  3,  0, 12,  8,  4, 10, 14,  2,  6, 40, 29, 46, 39, 43, 33, 47, 27, 42, 37, 44, 31, 41, 25, 45, 35, 34, 30, 26, 38, 24, 28, 32, 36},
  {21, 14, 19,  4, 22,  2, 18,  8, 23,  6, 17, 12, 20, 10, 16,  0,  1, 13,  9,  5, 11, 15,  3,  7, 41, 30, 47, 36, 40, 34, 44, 24, 43, 38, 45, 28, 42, 26, 46, 32, 35, 31, 27, 39, 25, 29, 33, 37},
  {22, 15, 16,  5, 23,  3, 19,  9, 20,  7, 18, 13, 21, 11, 17,  1,  2, 14, 10,  6,  8, 12,  0,  4, 42, 31, 44, 37, 41, 35, 45, 25, 40, 39, 46, 29, 43, 27, 47, 33, 32, 28, 24, 36, 26, 30, 34, 38},
  {23, 12, 17,  6, 20,  0, 16, 10, 21,  4, 19, 14, 22,  8, 18,  2,  3, 15, 11,  7,  9, 13,  1,  5, 43, 28, 45, 38, 42, 32, 46, 26, 41, 36, 47, 30, 40, 24, 44, 34, 33, 29, 25, 37, 27, 31, 35, 39},
  {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
  {25, 26, 27, 24, 29, 30, 31, 28, 33, 34, 35, 32, 37, 38, 39, 36, 41, 42, 43, 40, 45, 46, 47, 44,  1,  2,  3,  0,  5,  6,  7,  4,  9, 10, 11,  8, 13, 14, 15, 12, 17, 18, 19, 16, 21, 22, 23, 20},
  {26, 27, 24, 25, 30, 31, 28, 29, 34, 35, 32, 33, 38, 39, 36, 37, 42, 43, 40, 41, 46, 47, 44, 45,  2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13, 18, 19, 16, 17, 22, 23, 20, 21},
  {27, 24, 25, 26, 31, 28, 29, 30, 35, 32, 33, 34, 39, 36, 37, 38, 43, 40, 41, 42, 47, 44, 45, 46,  3,  0,  1,  2,  7,  4,  5,  6, 11,  8,  9, 10, 15, 12, 13, 14, 19, 16, 17, 18, 23, 20, 21, 22},
  {28, 45, 38, 43, 32, 46, 26, 42, 36, 47, 30, 41, 24, 44, 34, 40, 29, 25, 37, 33, 31, 35, 39, 27, 12, 17,  6, 23,  0, 16, 10, 20,  4, 19, 14, 21,  8, 18,  2, 22, 15, 11,  7,  3, 13,  1,  5,  9},
  {29, 46, 39, 40, 33, 47, 27, 43, 37, 44, 31, 42, 25, 45, 35, 41, 30, 26, 38, 34, 28, 32, 36, 24, 13, 18,  7, 20,  1, 17, 11, 21,  5, 16, 15, 22,  9, 19,  3, 23, 12,  8,  4,  0, 14,  2,  6, 10},
  {30, 47, 36, 41, 34, 44, 24, 40, 38, 45, 28, 43, 26, 46, 32, 42, 31, 27, 39, 35, 29, 33, 37, 25, 14, 19,  4, 21,  2, 18,  8, 22,  6, 17, 12, 23, 10, 16,  0, 20, 13,  9,  5,  1, 15,  3,  7, 11},
  {31, 44, 37, 42, 35, 45, 25, 41, 39, 46, 29, 40, 27, 47, 33, 43, 28, 24, 36, 32, 30, 34, 38, 26, 15, 16,  5, 22,  3, 19,  9, 23,  7, 18, 13, 20, 11, 17,  1, 21, 14, 10,  6,  2, 12,  0,  4,  8},
  {32, 35, 34, 33, 36, 39, 38, 37, 24, 27, 26, 25, 28, 31, 30, 29, 46, 45, 44, 47, 42, 41, 40, 43,  8, 11, 10,  9, 12, 15, 14, 13,  0,  3,  2,  1,  4,  7,  6,  5, 22, 21, 20, 23, 18, 17, 16, 19},
  {33, 32, 35, 34, 37, 36, 39, 38, 25, 24, 27, 26, 29, 28, 31, 30, 47, 46, 45, 44, 43, 42, 41, 40,  9,  8, 11, 10, 13, 12, 15, 14,  1,  0,  3,  2,  5,  4,  7,  6, 23, 22, 21, 20, 19, 18, 17, 16},
  {34, 33, 32, 35, 38, 37, 36, 39, 26, 25, 24, 27, 30, 29, 28, 31, 44, 47, 46, 45, 40, 43, 42, 41, 10,  9,  8, 11, 14, 13, 12, 15,  2,  1,  0,  3,  6,  5,  4,  7, 20, 23, 22, 21, 16, 19, 18, 17},
  {35, 34, 33, 32, 39, 38, 37, 36, 27, 26, 25, 24, 31, 30, 29, 28, 45, 44, 47, 46, 41, 40, 43, 42, 11, 10,  9,  8, 15, 14, 13, 12,  3,  2,  1,  0,  7,  6,  5,  4, 21, 20, 23, 22, 17, 16, 19, 18},
  {36, 41, 30, 47, 24, 40, 34, 44, 28, 43, 38, 45, 32, 42, 26, 46, 39, 35, 31, 27, 37, 25, 29, 33,  4, 21, 14, 19,  8, 22,  2, 18, 12, 23,  6, 17,  0, 20, 10, 16,  5,  1, 13,  9,  7, 11, 15,  3},
  {37, 42, 31, 44, 25, 41, 35, 45, 29, 40, 39, 46, 33, 43, 27, 47, 36, 32, 28, 24, 38, 26, 30, 34,  5, 22, 15, 16,  9, 23,  3, 19, 13, 20,  7, 18,  1, 21, 11, 17,  6,  2, 14, 10,  4,  8, 12,  0},
  {38, 43, 28, 45, 26, 42, 32, 46, 30, 41, 36, 47, 34, 40, 24, 44, 37, 33, 29, 25, 39, 27, 31, 35,  6, 23, 12, 17, 10, 20,  0, 16, 14, 21,  4, 19,  2, 22,  8, 18,  7,  3, 15, 11,  5,  9, 13,  1},
  {39, 40, 29, 46, 27, 43, 33, 47, 31, 42, 37, 44, 35, 41, 25, 45, 38, 34, 30, 26, 36, 24, 28, 32,  7, 20, 13, 18, 11, 21,  1, 17, 15, 22,  5, 16,  3, 23,  9, 19,  4,  0, 12,  8,  6, 10, 14,  2},
  {40, 29, 46, 39, 43, 33, 47, 27, 42, 37, 44, 31, 41, 25, 45, 35, 34, 30, 26, 38, 24, 28, 32, 36, 20, 13, 18,  7, 21,  1, 17, 11, 22,  5, 16, 15, 23,  9, 19,  3,  0, 12,  8,  4, 10, 14,  2,  6},
  {41, 30, 47, 36, 40, 34, 44, 24, 43, 38, 45, 28, 42, 26, 46, 32, 35, 31, 27, 39, 25, 29, 33, 37, 21, 14, 19,  4, 22,  2, 18,  8, 23,  6, 17, 12, 20, 10, 16,  0,  1, 13,  9,  5, 11, 15,  3,  7},
  {42, 31, 44, 37, 41, 35, 45, 25, 40, 39, 46, 29, 43, 27, 47, 33, 32, 28, 24, 36, 26, 30, 34, 38, 22, 15, 16,  5, 23,  3, 19,  9, 20,  7, 18, 13, 21, 11, 17,  1,  2, 14, 10,  6,  8, 12,  0,  4},
  {43, 28, 45, 38, 42, 32, 46, 26, 41, 36, 47, 30, 40, 24, 44, 34, 33, 29, 25, 37, 27, 31, 35, 39, 23, 12, 17,  6, 20,  0, 16, 10, 21,  4, 19, 14, 22,  8, 18,  2,  3, 15, 11,  7,  9, 13,  1,  5},
  {44, 37, 42, 31, 45, 25, 41, 35, 46, 29, 40, 39, 47, 33, 43, 27, 24, 36, 32, 28, 34, 38, 26, 30, 16,  5, 22, 15, 19,  9, 23,  3, 18, 13, 20,  7, 17,  1, 21, 11, 10,  6,  2, 14,  0,  4,  8, 12},
  {45, 38, 43, 28, 46, 26, 42, 32, 47, 30, 41, 36, 44, 34, 40, 24, 25, 37, 33, 29, 35, 39, 27, 31, 17,  6, 23, 12, 16, 10, 20,  0, 19, 14, 21,  4, 18,  2, 22,  8, 11,  7,  3, 15,  1,  5,  9, 13},
  {46, 39, 40, 29, 47, 27, 43, 33, 44, 31, 42, 37, 45, 35, 41, 25, 26, 38, 34, 30, 32, 36, 24, 28, 18,  7, 20, 13, 17, 11, 21,  1, 16, 15, 22,  5, 19,  3, 23,  9,  8,  4,  0, 12,  2,  6, 10, 14},
  {47, 36, 41, 30, 44, 24, 40, 34, 45, 28, 43, 38, 46, 32, 42, 26, 27, 39, 35, 31, 33, 37, 25, 29, 19,  4, 21, 14, 18,  8, 22,  2, 17, 12, 23,  6, 16,  0, 20, 10,  9,  5,  1, 13,  3,  7, 11, 15}
};

symmetries_t multiplySymmetries(symmetries_t s1, symmetries_t s2) {

  symmetries_t s = 0;

  int i, a1, a2;

  for (int pos = 0; pos < 24; pos++)
    if ((s & (1 << pos)) == 0) {

      for (a1 = 1; a1 < 48; a1++)
        if (s1 & (((symmetries_t)1) << a1)) {
          i = transMult[pos][a1];
          if (i > pos)
            s |= (((symmetries_t)1) << i);
        }

      for (a2 = 1; a2 < 48; a2++)
        if (s2 & (((symmetries_t)1) << a2)) {
          i = transMult[pos][a2];
          if (i > pos)
            s |= (((symmetries_t)1) << i);
        }

      for (a1 = 1; a1 < 48; a1++)
        for (a2 = 1; a2 < 48; a2++)
          if ((s1 & (((symmetries_t)1) << a1)) && (s2 & (((symmetries_t)1) << a2))) {
            i = transMult[transMult[pos][a1]][a2];
            if (i > pos)
              s |= (((symmetries_t)1) << i);

            i = transMult[transMult[pos][a2]][a1];
            if (i > pos)
              s |= (((symmetries_t)1) << i);
          }
    }

  return s;
}

bool symmetrieContainsTransformation(symmetries_t s, unsigned int t) {

  return ((s & (1 << t)) != 0);
}

unsigned int numSymmetries(symmetries_t s) {

  s -= ((s >> 1) & (symmetries_t)0x5555555555555555ll);
  s = (((s >> 2) & (symmetries_t)0x3333333333333333ll) + (s & (symmetries_t)0x3333333333333333ll));
  s = (((s >> 4) + s) & (symmetries_t)0x0f0f0f0f0f0f0f0fll);
  s += (s >> 8);
  s += (s >> 16);
  s += (s >> 32);
  return (unsigned int)(s & 0x3f);
}


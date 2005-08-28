#include "ps3dloader.h"

#include <fstream>
#include <sstream>

/* either return a puzzle, or nil, when failed */
puzzle_c * loadPuzzlerSolver3D(std::istream * str) {

  puzzle_c * p = new puzzle_c();

  p->addProblem();
  p->probSetName(0, "Problem");

  std::string line;

  int state = 0;
  int linenum = 0;
  int sx, sy, sz;
  int piece;

  while (getline(*str,line,'\n'))  {

    switch (state) {

    case 0:

      if (line.substr(0, 6) == "PIECE ") {
        state = 1;
        line = line.substr(6, 11);
      } else if (line.substr(0, 7) == "RESULT ") {
        state = 2;
        line = line.substr(7, 11);
      }

      if (state != 0) {

        std::istringstream s(line);

        char c;

        s >> sx >> c >> sy >> c >> sz;

        if ((sx > 500) || (sy > 500) || (sz > 500)) {
          delete p;
          return 0;
        }

        piece = p->addShape(sx, sy, sz);

        if (state == 2)
          p->probSetResult(0, piece);
        else
          p->probAddShape(0, piece, 1);

        linenum = 0;
      }

      break;

    case 1:
    case 2:

      for (int z = 0; z < sz; z++)
        for (int x = 0; x < sx; x++) {
          char c = line[z*(sx+1)+x];
          if (c == ',') {
            delete p;
            return 0;
          }
          if (c != ' ')
            p->getShape(piece)->setState(x, linenum, z, pieceVoxel_c::VX_FILLED);
        }

      linenum ++;
      if (linenum >= sy) {
        state = 0;
      }

      break;
    }
  }

  return p;
}

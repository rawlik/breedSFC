#include <iostream>
#include <array>

using namespace std;

#include "TApplication.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TPolyLine3D.h"
#include "TPad.h"
#include "TView.h"
#include "TPolyMarker3D.h"

enum face_t {
   LEFT = 0,
   FRONT = 1,
   RIGHT = 2,
   BACK = 3,
   TOP = 4,
   BOTTOM = 5
};

class SFCCube;

class Tile {
public:
   SFCCube * cube;
   face_t face;
   int x;
   int y;
   char ch = '#';

   array< Tile*, 4 > neighbours();
};

class Face {
public:
   int Nrows;
   int Ncols;
   Tile * tiles;

   Face(int Nrows, int Ncols);
   ~Face();
   Tile * i(int row, int column);
};

class SFCCube {
public:
   int N;
   Face side;
   Face top_cap;
   Face bot_cap;

   SFCCube(int N);
   Tile * i(face_t face, int x, int y);
   array< Tile*, 4 > neighbours(face_t face, int x, int y);
   void print();
};

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

class Square {
public:
   SFCCube * cube;
   face_t face;
   int x;
   int y;
   char ch = '#';

   array< Square*, 4 > neighbours();
};

class Face {
public:
   int Nrows;
   int Ncols;
   Square * squares;

   Face(int Nrows, int Ncols);
   ~Face();
   Square * i(int row, int column);
};

class SFCCube {
public:
   int N;
   Face side;
   Face top_cap;
   Face bot_cap;

   SFCCube(int N);
   Square * i(face_t face, int x, int y);
   array< Square*, 4 > neighbours(face_t face, int x, int y);
   void print();
};

#include <cmath>
#include <iostream>
#include <cassert>
#include <array>
#include <vector>

using namespace std;

typedef double coordinate;
typedef array<coordinate, 3> point3D;
typedef array<coordinate, 2> point2D;
typedef array<coordinate, 3> vector3D;

bool eq(coordinate a, coordinate b);

enum face_t {
   LEFT = 0,
   FRONT = 1,
   RIGHT = 2,
   BACK = 3,
   TOP = 4,
   BOTTOM = 5
};

class SFCCube;
class Tile;
class Face;

class Tile {
public:
   SFCCube * cube;
   face_t face;
   int x;
   int y;
   char ch = '#';

   array< Tile*, 4 > neighbours();

   vector3D getNormal();
   point3D getCenter();
   array<point3D, 4> getPoints();
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
   vector<Tile*> getTiles();
};

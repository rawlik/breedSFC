#include <cmath>
#include <iostream>
#include <cassert>
#include <array>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// ROOT includes
#include <Math/Point3D.h>
#include <Math/Vector3D.h>
#include <Math/Point2D.h>
#include <Math/Vector2D.h>
#include <Math/RotationX.h>
#include <Math/RotationY.h>
#include <Math/RotationZ.h>
#include <Math/AxisAngle.h>
#include <Math/Rotation3D.h>
#include <TMath.h>

using namespace ROOT::Math;
using namespace TMath;

const auto pi = TMath::Pi();
const auto pi2 = TMath::Pi() / 2.;

typedef double coordinate;

bool eq(coordinate a, coordinate b);

Rotation3D RotateWireToZ(XYZPoint wire_a, XYZPoint wire_b);

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
   int l;
   int m;
   char ch = '#';

   array< Tile*, 4 > neighbours();

   XYVector getFaceVector();
   XYZVector getNormal();
   Rotation3D getTransformFromFront();
   XYZPoint getCenter();
   string str();
   array<XYZPoint, 4> getPoints();
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
   Tile * i(face_t face, int l, int m);
   array< Tile*, 4 > neighbours(face_t face, int l, int m);
   void print();
   vector<Tile*> getTiles();
};

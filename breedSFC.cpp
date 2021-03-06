#include "breedSFC.h"

bool eq(coordinate a, coordinate b) {
   return fabs(a - b) < coordinate(1e-6);
}


Rotation3D WireToZRotation(XYZPoint wire_a, XYZPoint wire_b) {
   coordinate dx = wire_b.x() - wire_a.x();
   coordinate dy = wire_b.y() - wire_a.y();
   coordinate dz = wire_b.z() - wire_a.z();

   if (dx == 0 and dy == 0) {
      // identity rotation
      return Rotation3D();
   } else {
      coordinate xp = dy / sqrt(Power(dx, 2) + Power(dy, 2));
      coordinate yp = sqrt(1 - Power(xp, 2));

      XYZVector rotation_vector = XYZVector(xp, yp, 0);

      coordinate angle;

      if (dz == 0) {
         angle = pi2;
      } else {
         angle = atan(sqrt(Power(dx, 2) + Power(dy, 2)) / dz);
      }

      return Rotation3D(AxisAngle(rotation_vector, angle));
   }
}

Translation3D ZWireToOriginTranslation(XYZPoint wire_a, XYZPoint wire_b) {
   return Translation3D(-wire_a.x(), -wire_a.y(), 0);
}

Transform3D WireAndPointTarnsform(XYZPoint wire_a, XYZPoint wire_b, XYZPoint point) {
   auto rotation = WireToZRotation(wire_a, wire_b);

   wire_a = rotation * wire_a;
   wire_b = rotation * wire_b;

   auto translation = ZWireToOriginTranslation(wire_a, wire_b);

   wire_a = translation * wire_a;
   wire_b = translation * wire_b;
   point = (translation * rotation) * point;

   auto rotation2 = RotationZ();

   if (!eq(point.x(), 0)) {
      rotation2 = RotationZ(-atan(point.y() / point.x()));
   }

   return rotation2 * translation * rotation;
}

XYZVector BfromWire(XYZPoint wire_a, XYZPoint wire_b, XYZPoint point){
   auto transform = WireAndPointTarnsform(wire_a, wire_b, point);

   wire_a = transform * wire_a;
   wire_b = transform * wire_b;
   point = transform * point;

   auto Bx = TF1("Bx", "[0] * ([0]^2 + x^2)^(-3/2)", wire_a.z(), wire_b.z());
   auto Bz = TF1("Bz", "x   * ([0]^2 + x^2)^(-3/2)", wire_a.z(), wire_b.z());
   Bx.SetParameter(0, point.x());
   Bz.SetParameter(0, point.x());
   auto wrappedBx = WrappedTF1(Bx);
   auto wrappedBz = WrappedTF1(Bz);

   GSLIntegrator integrator(IntegrationOneDim::kADAPTIVE);
   integrator.SetRelTolerance(0.001);

   integrator.SetFunction(wrappedBx);
   auto Bx_value = integrator.Integral(wire_a.z(), wire_b.z());

   integrator.SetFunction(wrappedBz);
   auto Bz_value = integrator.Integral(wire_a.z(), wire_b.z());

   XYZVector B({ Bx_value, 0, Bz_value });

   B = transform.Inverse() * B;

   return B;
}

array< Tile*, 4 > Tile::neighbours() {
   return cube->neighbours(face, l, m);
}

XYVector Tile::getFaceVector() {
   if (cube->N == 1) return XYVector(0, 0);

   return XYVector(coordinate(l) / coordinate(cube->N) - 0.5 + coordinate(1) / coordinate(2 * cube->N),
                   coordinate(m) / coordinate(cube->N) - 0.5 + coordinate(1) / coordinate(2 * cube->N));
}

string Tile::str() {
   stringstream str;

   str << "(";

   if      (face == LEFT)   str << "left";
   else if (face == FRONT)  str << "front";
   else if (face == RIGHT)  str << "right";
   else if (face == BACK)   str << "back";
   else if (face == TOP)    str << "top";
   else if (face == BOTTOM) str << "bottom";
   else ;

   str << ", " << l << ", " << m << ")";
   return str.str();
}

Rotation3D Tile::getTransformFromFront() {
   if      (face == LEFT)   return Rotation3D(RotationZ(-pi2));
   else if (face == FRONT)  return Rotation3D();
   else if (face == RIGHT)  return Rotation3D(RotationZ(pi2));
   else if (face == BACK)   return Rotation3D(RotationZ(pi));
   else if (face == TOP)    return Rotation3D(RotationX(-pi2));
   else if (face == BOTTOM) return Rotation3D(RotationX(pi2));
   else  // will not happen
      return Rotation3D();
}

XYZVector Tile::getNormal() {
   return getTransformFromFront() * XYZVector({0, -1, 0});
}

XYZPoint Tile::getCenter() {
   auto v = getFaceVector();

   return getTransformFromFront() * XYZPoint(v.x(), -0.5, v.y());
}

array<XYZPoint, 4> Tile::getPoints() {
   coordinate a = coordinate(1) / (2 * coordinate(cube->N));

   array<XYZVector, 4> displacements = { XYZVector(-a, 0, -a),
                                         XYZVector(-a, 0,  a),
                                         XYZVector( a, 0,  a),
                                         XYZVector( a, 0, -a) };

   int i = 0;
   array<XYZPoint, 4> points;

   Rotation3D r = getTransformFromFront();

   for ( auto displacement : displacements) {
      points[i++] = r * ((r.Inverse() * getCenter()) + displacement);
   }                           

   return points;
}

array<array<XYZPoint, 2>, 4> Tile::getWires() {
   array<array<XYZPoint, 2>, 4> wires;

   auto points = getPoints();

   wires[0] = { points[0], points[1] };
   wires[1] = { points[1], points[2] };
   wires[2] = { points[2], points[3] };
   wires[3] = { points[3], points[0] };

   return wires;
}

XYZVector Tile::Bresponse(XYZPoint point){
   XYZVector B;

   for (auto wire : getWires()) {
      B += BfromWire(wire[0], wire[1], point);
   }

   return B;
}


Face::Face(int Nrows, int Ncols) : Nrows(Nrows), Ncols(Ncols) {
   tiles = new Tile[Nrows * Ncols];
}

Face::~Face() {
   delete[] tiles;
}

Tile * Face::i(int row, int column) {
   return &tiles[row * Ncols + column];
}

SFCCube::SFCCube(int N) : N(N), side(N, 4 * N), top_cap(N, N), bot_cap(N, N) {
   for (int face = 0; face < 6; ++face) {
      for (int l = 0; l < N; ++l) {
         for (int m = 0; m < N; ++m) {
            Tile * tile = i(face_t(face), l, m);

            tile->cube = this;
            tile->face = face_t(face);
            tile->l = l;
            tile->m = m;
         }
      }
   }
}

Tile * SFCCube::i(face_t face, int l, int m) {
   if (face == LEFT || face == FRONT || face == RIGHT || face == BACK) {
      return side.i(N - m - 1, int(face) * N + l);
   } else if (face == TOP) {
      return top_cap.i(N - m - 1, l);
   } else {
      // face = BOTTOM
      return bot_cap.i(N - m - 1, l);
   }
}

array< Tile*, 4 > SFCCube::neighbours(face_t face, int l, int m) {
   l = l % N;
   m = m % N;

   Tile * top;
   Tile * bottom;
   Tile * left;
   Tile * right;

   if (face == LEFT || face == FRONT || face == RIGHT || face == BACK) {
      // the tile is on the side of the cube
      int row = N - m - 1;
      int col = int(face) * N + l;

      // remember about weird modulo for negative in C!
      left = side.i(row, (4 * N + col - 1) % (4 * N));
      right = side.i(row, (col + 1) % (4 * N));

      if (row > 0) {
         top = side.i(row - 1, col);
      } else {
         // top neighbour on the top cap
         if      (face == 0) top = top_cap.i(l,     0);
         else if (face == 1) top = top_cap.i(N-1,   l);
         else if (face == 2) top = top_cap.i(N-l-1, N-1);
         else if (face == 3) top = top_cap.i(0,     N-l-1);
      }

      if (row < N - 1) {
         bottom = side.i(row + 1, col);
      } else {
         // bottom neighbour in on the bottom cap
         if      (face == 0) bottom = bot_cap.i(N-l-1, 0);
         else if (face == 1) bottom = bot_cap.i(0,     l);
         else if (face == 2) bottom = bot_cap.i(l,     N-1);
         else if (face == 3) bottom = bot_cap.i(N-1,   N-l-1);            
      }
   } else if (face == TOP) {
      // the top cap
      if (l == 0) left = i(LEFT, N - 1 - m, N - 1);
      else        left = i(TOP, l - 1, m);

      if (l == N - 1) right = i(RIGHT, m, N - 1);
      else            right = i(TOP, l + 1, m);

      if (m == 0) bottom = i(FRONT, l, N - 1);
      else        bottom = i(TOP, l, m - 1);

      if (m == N - 1) top = i(BACK, N - l - 1, N - 1);
      else            top = i(TOP, l, m + 1);       
   } else if (face == BOTTOM) {
      if (l == 0) left = i(LEFT, m, 0);
      else        left = i(BOTTOM, l - 1, m);

      if (l == N - 1) right = i(RIGHT, N - m - 1, 0);
      else            right = i(BOTTOM, l + 1, m);

      if (m == 0) bottom = i(BACK, N - l - 1, 0);
      else        bottom = i(BOTTOM, l, m - 1);

      if (m == N - 1) top = i(FRONT, l, 0);
      else            top = i(BOTTOM, l, m + 1);       

   }

   return array< Tile*, 4 >{ {left, top, right, bottom} };
}

void SFCCube::print() {
   // print top cap
   for (int row = 0; row < N; ++row) {
      for (int i = 0; i < N; ++i) {
         cout << "  ";
      }
      for (int col = 0; col < N; ++col) {
         cout << top_cap.i(row,col)->ch << " ";
      }
      cout << endl;
   }

   // print side
   for (int row = 0; row < N; ++row) {
      for (int col = 0; col < 4 * N; ++col) {
         cout << side.i(row,col)->ch << " ";
      }
      cout << endl;
   }

   // print bottom cap
   for (int row = 0; row < N; ++row) {
      for (int i = 0; i < N; ++i) {
         cout << "  ";
      }
      for (int col = 0; col < N; ++col) {
         cout << bot_cap.i(row,col)->ch << " ";
      }
      cout << endl;
   }
}

vector<Tile*> SFCCube::getTiles() {
   vector<Tile*> tiles;

   for (int face = 0; face < 6; ++face)
      for (int x = 0; x < N; ++x)
         for (int y = 0; y < N; ++y)
            tiles.push_back(i(face_t(face), x, y));

   return tiles;
}

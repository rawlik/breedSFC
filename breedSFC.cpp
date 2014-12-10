#include "breedSFC.h"

bool eq(coordinate a, coordinate b) {
   return fabs(a - b) < coordinate(1e-6);
}

array< Tile*, 4 > Tile::neighbours() {
   return cube->neighbours(face, x, y);
}

vector3D Tile::getNormal() {
   if      (face == LEFT)   return vector3D({-1, 0, 0});
   else if (face == FRONT)  return vector3D({0, -1, 0});
   else if (face == RIGHT)  return vector3D({1, 0, 0});
   else if (face == BACK)   return vector3D({0, 1, 0});
   else if (face == TOP)    return vector3D({0, 0, 1});
   else if (face == BOTTOM) return vector3D({0, 0, -1});
   else  // will not happen
      return vector3D({0, 0, 0});
}

point3D Tile::getCenter() {
   coordinate x_face, y_face;

   coordinate half_face = coordinate(cube->N) / coordinate(2);

   x_face = x - half_face;
   y_face = y - half_face;

   if      (face == LEFT)   return point3D({-half_face, -x_face, y_face});
   else if (face == FRONT)  return point3D({x_face, -half_face, y_face});
   else if (face == RIGHT)  return point3D({half_face, x_face, y_face});
   else if (face == BACK)   return point3D({-x_face, half_face, y_face});
   else if (face == TOP)    return point3D({x_face, y_face, half_face});
   else if (face == BOTTOM) return point3D({-y_face, x_face, -half_face});
   else  // will not happen
      return point3D({0, 0, 0});
}

array<point3D, 4> Tile::getPoints() {
   array<point3D, 4> points;
   int i = 0;

   for (coordinate dx = -0.5; dx < 0.6; dx += 1.0) {
      if (!eq(getNormal()[0], 0))
         dx = 0;

      for (coordinate dy = -0.5; dy < 0.6; dy += 1.0) {
         if (!eq(getNormal()[1], 0))
            dy = 0;

         for (coordinate dz = -0.5; dz < 0.6; dz += 1.0) {
            if (!eq(getNormal()[2], 0))
               dz = 0;

            points[i++] = point3D({getCenter()[0] + dx,
                                   getCenter()[1] + dy,
                                   getCenter()[2] + dz});

            if (!eq(getNormal()[2], 0))
               break;
         }

         if (!eq(getNormal()[1], 0))
            break;
      }

      if (!eq(getNormal()[0], 0))
         break;
   }

   assert(i == 4);

   return points;
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
      for (int x = 0; x < N; ++x) {
         for (int y = 0; y < N; ++y) {
            Tile * tile = i(face_t(face), x, y);

            tile->cube = this;
            tile->face = face_t(face);
            tile->x = x;
            tile->y = y;
         }
      }
   }
}

Tile * SFCCube::i(face_t face, int x, int y) {
   if (face == LEFT || face == FRONT || face == RIGHT || face == BACK) {
      return side.i(N - y - 1, int(face) * N + x);
   } else if (face == TOP) {
      return top_cap.i(N - y - 1, x);
   } else {
      // face = BOTTOM
      return bot_cap.i(N - y - 1, x);
   }
}

array< Tile*, 4 > SFCCube::neighbours(face_t face, int x, int y) {
   x = x % N;
   y = y % N;

   Tile * top;
   Tile * bottom;
   Tile * left;
   Tile * right;

   if (face == LEFT || face == FRONT || face == RIGHT || face == BACK) {
      // the tile is on the side of the cube
      int row = N - y - 1;
      int col = int(face) * N + x;

      // remember about weird modulo for negative in C!
      left = side.i(row, (4 * N + col - 1) % (4 * N));
      right = side.i(row, (col + 1) % (4 * N));

      if (row > 0) {
         top = side.i(row - 1, col);
      } else {
         // top neighbour on the top cap
         if      (face == 0) top = top_cap.i(x,     0);
         else if (face == 1) top = top_cap.i(N-1,   x);
         else if (face == 2) top = top_cap.i(N-x-1, N-1);
         else if (face == 3) top = top_cap.i(0,     N-x-1);
      }

      if (row < N - 1) {
         bottom = side.i(row + 1, col);
      } else {
         // bottom neighbour in on the bottom cap
         if      (face == 0) bottom = bot_cap.i(N-x-1, 0);
         else if (face == 1) bottom = bot_cap.i(0,     x);
         else if (face == 2) bottom = bot_cap.i(x,     N-1);
         else if (face == 3) bottom = bot_cap.i(N-1,   N-x-1);            
      }
   } else if (face == TOP) {
      // the top cap
      if (x == 0) left = i(LEFT, N - 1 - y, N - 1);
      else        left = i(TOP, x - 1, y);

      if (x == N - 1) right = i(RIGHT, y, N - 1);
      else            right = i(TOP, x + 1, y);

      if (y == 0) bottom = i(FRONT, x, N - 1);
      else        bottom = i(TOP, x, y - 1);

      if (y == N - 1) top = i(BACK, N - x - 1, N - 1);
      else            top = i(TOP, x, y + 1);       
   } else if (face == BOTTOM) {
      if (x == 0) left = i(LEFT, y, 0);
      else        left = i(BOTTOM, x - 1, y);

      if (x == N - 1) right = i(RIGHT, N - y - 1, 0);
      else            right = i(BOTTOM, x + 1, y);

      if (y == 0) bottom = i(BACK, N - x - 1, 0);
      else        bottom = i(BOTTOM, x, y - 1);

      if (y == N - 1) top = i(FRONT, x, 0);
      else            top = i(BOTTOM, x, y + 1);       

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

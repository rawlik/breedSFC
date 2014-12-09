#include "breedSFC.h"

array< Square*, 4 > Square::neighbours() {
   return cube->neighbours(face, x, y);
}

Face::Face(int Nrows, int Ncols) : Nrows(Nrows), Ncols(Ncols) {
   squares = new Square[Nrows * Ncols];
}

Face::~Face() {
   delete[] squares;
}

Square * Face::i(int row, int column) {
   return &squares[row * Ncols + column];
}

SFCCube::SFCCube(int N) : N(N), side(N, 4 * N), top_cap(N, N), bot_cap(N, N) {
   for (int face = 0; face < 6; ++face) {
      for (int x = 0; x < N; ++x) {
         for (int y = 0; y < N; ++y) {
            Square * square = i(face_t(face), x, y);

            square->cube = this;
            square->face = face_t(face);
            square->x = x;
            square->y = y;
         }
      }
   }
}

Square * SFCCube::i(face_t face, int x, int y) {
   if (face == LEFT || face == FRONT || face == RIGHT || face == BACK) {
      return side.i(N - y - 1, int(face) * N + x);
   } else if (face == TOP) {
      return top_cap.i(N - y - 1, x);
   } else {
      // face = BOTTOM
      return bot_cap.i(N - y - 1, x);
   }
}

array< Square*, 4 > SFCCube::neighbours(face_t face, int x, int y) {
   x = x % N;
   y = y % N;

   Square * top;
   Square * bottom;
   Square * left;
   Square * right;

   if (face == LEFT || face == FRONT || face == RIGHT || face == BACK) {
      // the square is on the side of the cube
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

   return array< Square*, 4 >{ {left, top, right, bottom} };
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

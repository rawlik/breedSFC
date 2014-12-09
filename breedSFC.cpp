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

bool test_neighbours(Square * square) {
   bool pass = true;

   for (auto neighbour : square->neighbours()) {
      int i = 0;
      
      for (auto next_neighbour : neighbour->neighbours()) {
         if (next_neighbour == square)
            i++;
      }

      if (i != 1){
         pass = false;
      }
   }

   return pass;
}


int main(int argc, char **argv) {
   int N = 5;

   SFCCube c = SFCCube(5);
   c.print();

   c.i(LEFT, 0, 0)->ch = '!';
   cout << endl;
   // c.neighbours(0, 2, 2)[0]->ch = '*';
   auto square = c.i(TOP, 2, 2);

   c.print();

   bool pass = true;
   for (int face = 0; face < 6; ++face) {
      for (int x = 0; x < N; ++x) {
         for (int y = 0; y < N; ++y) {
            if (!test_neighbours(c.i(face_t(face), x, y))) {
               cout << face << x << y;
               pass = false;
            }
         }
      }
   }

   if (pass) {
      cout << "PASSED!" << endl;
   }

   return 0;






   TApplication theApp("App", &argc, argv);

   // auto f = [=](double x){ return x * x; };

   // cout << f(4) << endl;

   TCanvas * c1 = new TCanvas("c1","PolyLine3D & PolyMarker3D Window",200,10,700,500);

   // create a pad
   // auto p1 = new TPad("p1","p1",0.05,0.02,0.95,0.82,46,3,1);
   TPad * p1 = new TPad("p1","p1",0.05,0.02,0.95,0.82,46,3,1);
   p1->Draw();
   p1->cd();

   // creating a view
   TView * view = TView::CreateView(1);
   view->SetRange(5,5,5,25,25,25);

   // create a first PolyLine3D
   TPolyLine3D *pl3d1 = new TPolyLine3D(5);

   // set points
   pl3d1->SetPoint(0, 10, 10, 10);
   pl3d1->SetPoint(1, 15, 15, 10);
   pl3d1->SetPoint(2, 20, 15, 15);
   pl3d1->SetPoint(3, 20, 20, 20);
   pl3d1->SetPoint(4, 10, 10, 20);
   // set attributes
   pl3d1->SetLineWidth(3);
   pl3d1->SetLineColor(5);

   // create a second PolyLine3D
   TPolyLine3D *pl3d2 = new TPolyLine3D(4);

   // set points
   pl3d2->SetPoint(0, 5, 10, 5);
   pl3d2->SetPoint(1, 10, 15, 8);
   pl3d2->SetPoint(2, 15, 15, 18);
   pl3d2->SetPoint(3, 5, 20, 20);
   pl3d2->SetPoint(4, 10, 10, 5);

   // set attributes
   pl3d2->SetLineWidth(5);
   pl3d2->SetLineColor(2);

   // create a first PolyMarker3D
   TPolyMarker3D *pm3d1 = new TPolyMarker3D(12);

   // set points
   pm3d1->SetPoint(0, 10, 10, 10);
   pm3d1->SetPoint(1, 11, 15, 11);
   pm3d1->SetPoint(2, 12, 15, 9);
   pm3d1->SetPoint(3, 13, 17, 20);
   pm3d1->SetPoint(4, 14, 16, 15);
   pm3d1->SetPoint(5, 15, 20, 15);
   pm3d1->SetPoint(6, 16, 18, 10);
   pm3d1->SetPoint(7, 17, 15, 10);
   pm3d1->SetPoint(8, 18, 22, 15);
   pm3d1->SetPoint(9, 19, 28, 25);
   pm3d1->SetPoint(10, 20, 12, 15);
   pm3d1->SetPoint(11, 21, 12, 15);

   // set marker size, color & style
   pm3d1->SetMarkerSize(2);
   pm3d1->SetMarkerColor(4);
   pm3d1->SetMarkerStyle(2);

   // create a second PolyMarker3D
   TPolyMarker3D *pm3d2 = new TPolyMarker3D(8);

   pm3d2->SetPoint(0, 22, 15, 15);
   pm3d2->SetPoint(1, 23, 18, 21);
   pm3d2->SetPoint(2, 24, 26, 13);
   pm3d2->SetPoint(3, 25, 17, 15);
   pm3d2->SetPoint(4, 26, 20, 15);
   pm3d2->SetPoint(5, 27, 15, 18);
   pm3d2->SetPoint(6, 28, 20, 10);
   pm3d2->SetPoint(7, 29, 20, 20);

   // set marker size, color & style
   pm3d2->SetMarkerSize(2);
   pm3d2->SetMarkerColor(1);
   pm3d2->SetMarkerStyle(8);

   // draw
   pl3d1->Draw();
   pl3d2->Draw();
   pm3d1->Draw();
   pm3d2->Draw();
   //
   // draw a title/explanation in the canvas pad
   c1->cd();
   TPaveText *title = new TPaveText(0.1,0.85,0.9,0.97);
   title->SetFillColor(24);
   title->AddText("Examples of 3-D primitives");
   TText *click=title->AddText("Click anywhere on the picture to rotate");
   click->SetTextColor(4);
   title->Draw();
   theApp.Run(kTRUE);

   cout << "hello" << endl;

   theApp.Run();

   return 0;
}

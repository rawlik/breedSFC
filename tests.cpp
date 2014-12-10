#include <iostream>
#include <vector>
#include <string>

#include "breedSFC.h"

bool test_tile_neighbours(Tile * tile) {
   bool pass = true;

   for (auto neighbour : tile->neighbours()) {
      int i = 0;
      
      for (auto next_neighbour : neighbour->neighbours()) {
         if (next_neighbour == tile)
            i++;
      }

      if (i != 1){
         pass = false;
      }
   }

   return pass;
}

bool test_neighbours(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;
   
   for (int face = 0; face < 6; ++face) {
      for (int x = 0; x < N; ++x) {
         for (int y = 0; y < N; ++y) {
            if (!test_tile_neighbours(c.i(face_t(face), x, y))) {
               cout << face << x << y;
               pass = false;
            }
         }
      }
   }

   return pass;
}

bool test_get_tiles(int N) {
   return SFCCube(N).getTiles().size() == N * N * 6;
}

bool test_centers(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      coordinate half_face = coordinate(c.N) / coordinate(2);
      coordinate face_coordinate;

      if      (tile->face == LEFT)   face_coordinate = -tile->getCenter()[0];
      else if (tile->face == FRONT)  face_coordinate = -tile->getCenter()[1];
      else if (tile->face == RIGHT)  face_coordinate =  tile->getCenter()[0];
      else if (tile->face == BACK)   face_coordinate =  tile->getCenter()[1];
      else if (tile->face == TOP)    face_coordinate =  tile->getCenter()[2];
      else if (tile->face == BOTTOM) face_coordinate = -tile->getCenter()[2];

      if (!eq(face_coordinate, half_face)) {
         pass = false;
      }
   }

   return pass;
}

bool test_points(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      coordinate half_face = coordinate(c.N) / coordinate(2);
      coordinate face_coordinate;

      for (auto point : tile->getPoints()) {
         if      (tile->face == LEFT)   face_coordinate = -point[0];
         else if (tile->face == FRONT)  face_coordinate = -point[1];
         else if (tile->face == RIGHT)  face_coordinate =  point[0];
         else if (tile->face == BACK)   face_coordinate =  point[1];
         else if (tile->face == TOP)    face_coordinate =  point[2];
         else if (tile->face == BOTTOM) face_coordinate = -point[2];

         if (!eq(face_coordinate, half_face)) {
            pass = false;
         }
      }
   }

   return pass;
}

bool test_all_sizes(bool (*test)(int), string name, vector<int> sizes) {
   bool all_pass = true;

   cout << "    " << name << ": ";

   for (auto N : sizes) {
      bool pass = test(N);

      if (pass)
         cout << "\033[1;32m" << "✓ ";
      else {
         all_pass = false;
         cout << "\033[1;31m" << "✘ ";
      }
   }

   cout << '\r';
   if (all_pass)
      cout << "\033[1;32m" << "✔";
   else
      cout << "\033[1;31m" << "✗ ";

   cout << "\033[0m" << endl;

   return all_pass;
}


int main(int argc, char **argv) {
   vector<int> Ns = {1, 2, 3, 4, 5, 10, 100};

   bool pass = true;

   pass = pass && test_all_sizes(test_get_tiles,  "get tiles test ", Ns);
   pass = pass && test_all_sizes(test_neighbours, "neighbours test", Ns);
   pass = pass && test_all_sizes(test_centers,    "centers test   ", Ns);
   pass = pass && test_all_sizes(test_points,     "points test    ", Ns);

   if (pass)
      cout << "\033[1;32m" << "✔";
   else
      cout << "\033[1;31m" << "✗ ";

   cout << "\033[0m" << endl;
}

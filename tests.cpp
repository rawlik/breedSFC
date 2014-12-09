#include <iostream>
#include <vector>

#include "breedSFC.h"

bool test_square_neighbours(Square * square) {
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

bool test_neighbours(int N) {
   SFCCube c = SFCCube(N);
   c.print();

   bool pass = true;
   
   for (int face = 0; face < 6; ++face) {
      for (int x = 0; x < N; ++x) {
         for (int y = 0; y < N; ++y) {
            if (!test_square_neighbours(c.i(face_t(face), x, y))) {
               cout << face << x << y;
               pass = false;
            }
         }
      }
   }

   return pass;
}


int main(int argc, char **argv) {
    bool pass = false;

    vector<int> Ns = {1, 2, 3, 4, 5, 10, 100};
    auto Pass = vector<bool>(Ns.size(), false);
    int i = 0;

    for (auto N : Ns) {
        Pass[i++] = test_neighbours(N);
    }

    for (auto pass : Pass) {
        cout << pass;
    }
}

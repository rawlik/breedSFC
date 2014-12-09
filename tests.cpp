#include <iostream>
#include <vector>
#include <string>

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

    pass = pass && test_all_sizes(test_neighbours, "neighbours test", Ns);

    if (pass)
        cout << "\033[1;32m" << "✔";
    else
        cout << "\033[1;31m" << "✗ ";

    cout << "\033[0m" << endl;

}

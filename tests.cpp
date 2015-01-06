#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "breedSFC.h"

ofstream logfile;

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
   
   for (auto tile : c.getTiles()) {
      if (!test_tile_neighbours(tile)) {
         pass = false;

         logfile << "The tile " << tile->str() << " is not a neighbour of ";
         logfile << "one of its neighbours." << endl; 
      }
   }

   return pass;
}

bool test_get_tiles(int N) {
   auto Ntiles = SFCCube(N).getTiles().size();
   bool pass = (Ntiles == N * N * 6);

   if (!pass) {
      logfile << "The cube has " << Ntiles << " tiles. Expected ";
      logfile << N * N * 6 << "." << endl;
   }

   return pass;
}

bool test_centers(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      coordinate face_coordinate;

      if      (tile->face == LEFT)   face_coordinate = -tile->getCenter().x();
      else if (tile->face == FRONT)  face_coordinate = -tile->getCenter().y();
      else if (tile->face == RIGHT)  face_coordinate =  tile->getCenter().x();
      else if (tile->face == BACK)   face_coordinate =  tile->getCenter().y();
      else if (tile->face == TOP)    face_coordinate =  tile->getCenter().z();
      else if (tile->face == BOTTOM) face_coordinate = -tile->getCenter().z();

      if (!eq(face_coordinate, 0.5)) {
         pass = false;

         logfile << "test_centers: face_coordinate is " << face_coordinate;
         logfile << ", expected " << 0.5 << ". The tile is " << tile->str() << ".";
         logfile << endl;
      }
   }

   return pass;
}

bool test_points(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      coordinate face_coordinate;

      for (auto point : tile->getPoints()) {
         if      (tile->face == LEFT)   face_coordinate = -point.x();
         else if (tile->face == FRONT)  face_coordinate = -point.y();
         else if (tile->face == RIGHT)  face_coordinate =  point.x();
         else if (tile->face == BACK)   face_coordinate =  point.y();
         else if (tile->face == TOP)    face_coordinate =  point.z();
         else if (tile->face == BOTTOM) face_coordinate = -point.z();

         if (!eq(face_coordinate, 0.5)) {
            pass = false;

            logfile << "Points of tile " << tile->str() << " don't lie on the face. ";
            logfile << "Coordinate is " << face_coordinate << " and expected 0.5";
            logfile << endl;
         }
      }
   }

   return pass;
}

bool test_tile_edge(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      coordinate face_coordinate;

      XYZVector previous;
      bool has_previous = false;

      for (auto point : tile->getPoints()) {
         if (has_previous) {
            XYZVector diff = XYZVector(point - previous);
            auto diff_len = sqrt(diff.Dot(diff));

            if (!eq(diff_len, coordinate(1) / c.N)) {
               logfile << "Length of an edge is " << diff_len << ". ";
               logfile << "Expected " << coordinate(1) / c.N << ". " << endl;

               pass = false;
            }

            has_previous = true;
            previous = point;
         }
      }
   }

   return pass;
}

bool test_getWires(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      for (auto wire : tile->getWires()) {
         auto wire_length = sqrt((wire[1] - wire[0]).Mag2());
         if (!eq(wire_length, coordinate(1) / c.N)) {
            logfile << "Length of a wire is " << wire_length << ". ";
            logfile << "Expected " << coordinate(1) / c.N << ". " << endl;

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
   logfile.open("tests.log");

   vector<int> Ns = {1, 2, 3, 4, 5, 10, 100};
   vector<bool> vpass;

   vpass.push_back(test_all_sizes(test_get_tiles,  "get tiles test ", Ns));
   vpass.push_back(test_all_sizes(test_neighbours, "neighbours test", Ns));
   vpass.push_back(test_all_sizes(test_centers,    "centers test   ", Ns));
   vpass.push_back(test_all_sizes(test_points,     "points test    ", Ns));
   vpass.push_back(test_all_sizes(test_tile_edge,  "edges test     ", Ns));
   vpass.push_back(test_all_sizes(test_getWires,   "getWires test  ", Ns));

   bool pass = true;
   for (auto p : vpass)
      pass = pass && p;

   // draw if passed all tests
   if (pass)
      cout << "\033[1;32m" << "✔";
   else {
      cout << "\033[1;31m" << "✗ ";
      cout << endl;
      cout << endl;
      cout << "Test failed. Output written to tests.log." << endl;
   }

   cout << "\033[0m" << endl;

   logfile.close();
}

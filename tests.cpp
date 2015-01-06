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

bool test_wire_Z_rotation(int N) {
   SFCCube c = SFCCube(N);

   bool pass = true;

   for (auto tile : c.getTiles()) {
      for (auto wire : tile->getWires()) {
         auto rotation = WireToZRotation(wire[0], wire[1]);

         auto new_wire0 = rotation * wire[0];
         auto new_wire1 = rotation * wire[1];

         if (!eq(new_wire0.x(), new_wire1.x())) {
            logfile << "The rotated wire is tilted around the x axis." << endl;

            pass = false;
         }

         if (!eq(new_wire0.y(), new_wire1.y())) {
            logfile << "The rotated wire is tilted around the y axis." << endl;

            pass = false;
         }
      }
   }

   return pass;
}

bool test_WireAndPointTarnsform(int N) {
   SFCCube c = SFCCube(N);

   TRandom r;

   bool pass = true;

   for (auto tile : c.getTiles()) {
      for (auto wire : tile->getWires()) {
         // XYZPoint point = XYZPoint(r.Rndm(), r.Rndm(), r.Rndm());
         XYZPoint point = XYZPoint(0, 0, 0);

         auto transform = WireAndPointTarnsform(wire[0], wire[1], point);

         auto new_point = transform * point;
         auto new_wire0 = transform * wire[0];
         auto new_wire1 = transform * wire[1];

         if (!eq(new_point.y(), 0)) {
            logfile << "The point after the WireAndPointTarnsform has y=" << new_point.y();
            logfile << ". 0 expected." << endl;

            pass = false;
         }

         for (auto new_wire : { new_wire0, new_wire1 }) {
            if (!eq(new_wire.x(), 0)) {
               logfile << "The wire point after the WireAndPointTarnsform has x=" << new_wire.x();
               logfile << ". 0 expected." << endl;

               pass = false;
            }

            if (!eq(new_wire.y(), 0)) {
               logfile << "The wire point after the WireAndPointTarnsform has y=" << new_wire.y();
               logfile << ". 0 expected." << endl;

               pass = false;
            }
         }
      }
   }

   return pass;
}

bool test_BfromWire_symmetry() {
   TRandom r;

   auto random_z = r.Rndm();
   auto random_x = r.Rndm();
   auto random_y = r.Rndm();

   bool pass = true;

   auto B = BfromWire({0, 0, -random_z}, {0, 0, random_z}, {random_x, random_y, 0});

   if (!eq(B.z(), 0)) {
      logfile << "BfromWire retured Bz = " << B.z() << " for symmetric wire. ";
      logfile << "Expected 0." << endl;

      pass = false;
   }

   return pass;
}

bool test_BfromWire_perpendicular() {
   TRandom r;

   auto random_z1 = r.Rndm();
   auto random_z2 = r.Rndm();
   auto random_z3 = r.Rndm();
   auto random_x = r.Rndm();
   auto random_y = r.Rndm();

   bool pass = true;

   auto B = BfromWire({0, 0, random_z1}, {0, 0, random_z2}, {random_x, random_y, random_z3});

   if (!eq(B.x() * random_y, B.y() * random_x)) {
      logfile << "B in the point is not perpendicular to the wire.";

      pass = false;
   }

   return pass;
}

bool test_simple(bool (*test)(), string name) {
   cout << "    " << name << ": ";

  bool pass = test();

   if (pass)
      cout << "\033[1;32m" << "✓ ";
   else {
      cout << "\033[1;31m" << "✘ ";
   }

   cout << '\r';
   if (pass)
      cout << "\033[1;32m" << "✔";
   else
      cout << "\033[1;31m" << "✗ ";

   cout << "\033[0m" << endl;

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

   // vector<int> Ns = {1, 2, 3, 4, 5, 10, 100};
   vector<int> Ns = {1};

   vector<bool> vpass;

   vpass.push_back(test_all_sizes(test_get_tiles,             "get tiles test                  ", Ns));
   vpass.push_back(test_all_sizes(test_neighbours,            "neighbours test                 ", Ns));
   vpass.push_back(test_all_sizes(test_centers,               "centers test                    ", Ns));
   vpass.push_back(test_all_sizes(test_points,                "points test                     ", Ns));
   vpass.push_back(test_all_sizes(test_tile_edge,             "edges test                      ", Ns));
   vpass.push_back(test_all_sizes(test_getWires,              "getWires test                   ", Ns));
   vpass.push_back(test_all_sizes(test_wire_Z_rotation,       "wire Z rotation test            ", Ns));
   vpass.push_back(test_all_sizes(test_WireAndPointTarnsform, "test_WireAndPointTarnsform test ", Ns));

   vpass.push_back(test_simple(test_BfromWire_symmetry,       "test_BfromWire symmetry test    "));
   vpass.push_back(test_simple(test_BfromWire_perpendicular,  "test_BfromWire perp.    test    "));

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

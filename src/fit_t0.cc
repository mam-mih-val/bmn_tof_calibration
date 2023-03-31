//
// Created by Misha on 3/30/2023.
//

#include <cassert>
#include <cstdlib>
#include <numeric>

#include "file_parser.h"
#include "fitter.h"

int main(int n_args, char** args){
  assert(n_args > 2);

  auto file_in = args[1];
  auto n_bins = atoi(args[2]);

  FileParser parser(file_in);
  std::vector<int> planes{6};
//  std::vector<int> planes(20, 0);
//  std::iota( planes.begin(), planes.end(), 0 );
  parser.ReadPlanes(planes);

  auto file_out = std::make_unique<TFile>( "out.root", "recreate" );
  file_out->cd();
  for( auto plane : planes ) {
    std::string str_plane = "plane_" + std::to_string(plane);
    file_out->mkdir(std::data(str_plane));
    file_out->cd(std::data(str_plane));
    int strip = 0;
    for (auto h2: parser.GetReadHistograms(plane)) {
      file_out->mkdir(std::data(str_plane + "/strip_" + std::to_string(strip)));
      file_out->cd(std::data(str_plane + "/strip_" + std::to_string(strip)));
      Fitter fitter(h2);
      fitter.SetNPoints(n_bins);
      fitter.Fit();
      fitter.Dump();
      strip++;
      file_out->cd(std::data(str_plane));
    }
  }
  file_out->Close();
  return 0;
}
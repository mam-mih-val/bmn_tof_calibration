//
// Created by Misha on 3/30/2023.
//

#include "file_parser.h"

void FileParser::ReadPlanes(std::vector<int> planes) {
  for( auto plane : planes ){
    std::vector<TH2D*> histograms;
    for( int strip=0; strip<48; strip++ ){
      std::string path="plane_"+std::to_string(plane)+"/plane"+std::to_string(plane)+"_strip"+std::to_string(strip);
      TH2D* h2{nullptr};
      file_in_->GetObject( path.c_str(), h2 );
      assert(h2);
      histograms.push_back(h2);
    }
    read_histograms_.insert( {plane, histograms} );
  }
}

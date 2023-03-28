//
// Created by Misha on 3/28/2023.
//

void converter(std::string str_file_in, std::string str_file_out="out.root"){
  ROOT::RDataFrame d( "TOF400", str_file_in );
  auto dd = d
          .Define("plane", "TOF400Conteiner.fPlane")
          .Define("strip", "TOF400Conteiner.fStrip")
          .Define("x", "TOF400Conteiner.fX")
          .Define("y", "TOF400Conteiner.fY")
          .Define("z", "TOF400Conteiner.fZ")
          .Define("time", "TOF400Conteiner.fTime")
          .Define("width", "TOF400Conteiner.fWidth")
          .Define("t0", "TOF400Conteiner.fTimeT0")
          .Define("w0", "TOF400Conteiner.fWidthT0")
          .Define("dt", "ROOT::VecOps::RVec<float> dt; for(int i=0; i<time.size(); ++i) dt.push_back(time[i]-t0[i]); return dt;")
          .Define("linear_id",
                  []( ROOT::VecOps::RVec<short> plane, ROOT::VecOps::RVec<int> strip ){
            // vectorizing the matrix of [n_planes x n_strips]
            ROOT::VecOps::RVec<int> linear{};
            for( size_t i=0; i<plane.size(); ++i ){
              auto p_id = plane.at(i);
              auto s_id = strip.at(i);
              auto l_id = p_id * 48 + s_id;
              linear.push_back(l_id);
            }
            return linear;
          },
          {"plane", "strip"})
          ;
  std::vector< std::vector< ROOT::RDF::RResultPtr<::TH2D> > > h2_tof_vs_tot;
  std::vector<std::string> cuts;
  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string str_coordinate = "plane"+std::to_string(plane_id)+"_strip"+std::to_string( strip_id );
      auto l_id = plane_id * 48 + strip_id;
      dd = dd.Define( str_coordinate, "plane == "+std::to_string(l_id) );
      cuts.emplace_back( str_coordinate );
    }
  }

  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    h2_tof_vs_tot.emplace_back();
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string str_coordinate = "plane"+std::to_string(plane_id)+"_strip"+std::to_string( strip_id );
      h2_tof_vs_tot.back().push_back( dd.Histo2D({str_coordinate.c_str(), ";T0 width;dt", 100, 30, 50, 100, -1000, 0}, "w0", "dt", str_coordinate) );
    }
  }

  auto file_out = std::make_unique<TFile>(str_file_out.c_str(), "recreate");
  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      h2_tof_vs_tot.at(plane_id).at(strip_id)->Write();
    }
  }
  file_out->Close();
}
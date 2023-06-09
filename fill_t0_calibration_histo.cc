//
// Created by Misha on 3/28/2023.
//

void fill_t0_calibration_histo(std::string str_list_in, std::string str_file_out="out.root"){
  TFileCollection collection( "collection", "", str_list_in.c_str() );
  auto* chain = new TChain( "TOF400" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );
  auto dd = d.Filter("1e4 < BC1Int && BC1Int < 4e4")
          .Define("plane", "TOF400Conteiner.fPlane")
          .Define("strip", "TOF400Conteiner.fStrip")
          .Define("x", "TOF400Conteiner.fX")
          .Define("y", "TOF400Conteiner.fY")
          .Define("z", "TOF400Conteiner.fZ")
          .Define("time", "TOF400Conteiner.fTime")
          .Define("width", "TOF400Conteiner.fWidth")
          .Define("t0", "TOF400Conteiner.fTimeT0")
          .Define("w0", "TOF400Conteiner.fWidthT0")
          .Define("linear_id",
                  []( ROOT::VecOps::RVec<short> plane, ROOT::VecOps::RVec<short> strip ){
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
          .Define("dt", "ROOT::VecOps::RVec<float> dt; for(int i=0; i<time.size(); ++i) dt.push_back(time[i]-t0[i]); return dt;")
  ;
  std::vector< std::vector< ROOT::RDF::RResultPtr<::TH2D> > > h2_tof_vs_tot;
  std::vector< std::vector< ROOT::RDF::RResultPtr<::TH1D> > > h1_tot;
  std::vector<std::string> cuts;
  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string str_coordinate = "plane"+std::to_string(plane_id)+"_strip"+std::to_string( strip_id );
      auto l_id = plane_id * 48 + strip_id;
      dd = dd.Define( str_coordinate,
                      [l_id](ROOT::VecOps::RVec<int> linear, ROOT::VecOps::RVec<float> width ){
        ROOT::VecOps::RVec<int> passed_cuts;
        for( int i=0; i<linear.size(); ++i ){
          if( linear.at(i) != l_id ){
            passed_cuts.push_back(0);
            continue;
          }
          if( width.at(i) < 24.5 ){
            passed_cuts.push_back(0);
            continue;
          }
          if( width.at(i) > 25.5 ){
            passed_cuts.push_back(0);
            continue;
          }
          passed_cuts.push_back(1);

        }
        return passed_cuts;
        }, {"linear_id", "width"});
      cuts.emplace_back( str_coordinate );
    }
  }

  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    h2_tof_vs_tot.emplace_back();
    h1_tot.emplace_back();
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string str_coordinate = "plane"+std::to_string(plane_id)+"_strip"+std::to_string( strip_id );
      h2_tof_vs_tot.back().push_back( dd.Histo2D({str_coordinate.c_str(), ";T0 width;dt", 512, 40, 52, 1536, -552, -516}, "w0", "dt", str_coordinate) );
      h1_tot.back().push_back( dd.Histo1D({std::data("width_"+str_coordinate), ";width;counts", 100, 0, 100}, "width", str_coordinate) );
    }
  }

  auto h2_tof_vs_tot_integral = dd.Histo2D({"tof_vs_tot_integral", ";T0 width;dt", 512, 40, 52, 1536, -552, -516}, "w0", "dt");

  auto file_out = std::make_unique<TFile>(str_file_out.c_str(), "recreate");
  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    std::string str_plane = "plane_"+std::to_string( plane_id );
    file_out->mkdir( str_plane.c_str() );
    file_out->cd( str_plane.c_str() );
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      h2_tof_vs_tot.at(plane_id).at(strip_id)->Write();
      h1_tot.at(plane_id).at(strip_id)->Write();
    }
    file_out->cd();
  }

  h2_tof_vs_tot_integral->Write();

  file_out->Close();
}
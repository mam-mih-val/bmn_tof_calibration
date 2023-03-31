//
// Created by Misha on 3/28/2023.
//

void fill_rpc_calibration_histo(std::string str_file_in, std::string str_file_out="out.root"){
  auto calib_file = std::make_unique<TFile>( "calib/out.root" );
  std::vector<TF1*> calib_functions{};
  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string name = "plane_"+std::to_string(plane_id)+"/strip_"+std::to_string(strip_id)+"/f1_tof_tot";
      TF1* f1{nullptr};
      calib_file->GetObject( name.c_str(), f1 );
      calib_functions.push_back(f1);
      std::cout << "plane: " << plane_id << " strip: " << strip_id << " f1: " << f1 << "\n";
    }
  }
  ROOT::RDataFrame d( "TOF400", str_file_in );
  std::vector< std::vector< ROOT::RDF::RResultPtr<::TH2D> > > h2_tof_vs_tot;
  auto dd = d.Filter("1e4 < BC1Int && BC1Int < 4e4")
          .Define("plane", "TOF400Conteiner.fPlane")
          .Define("strip", "TOF400Conteiner.fStrip")
          .Define("x", "TOF400Conteiner.fX")
          .Define("y", "TOF400Conteiner.fY")
          .Define("z", "TOF400Conteiner.fZ")
          .Define("time", "TOF400Conteiner.fTime")
          .Define("width", "TOF400Conteiner.fWidth")
          .Define("w0", "TOF400Conteiner.fWidthT0")
          .Define("t0", "TOF400Conteiner.fTimeT0")
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
          .Define("dt",
                  [calib_functions](ROOT::VecOps::RVec<float> vec_rpc_time, ROOT::VecOps::RVec<float> vec_t0, ROOT::VecOps::RVec<float> vec_w0, ROOT::VecOps::RVec<int> vec_id){
                    ROOT::VecOps::RVec<float> corrected_dt{};
                    float correction{};
                    for( int i=0; i<vec_t0.size(); ++i ){
                      auto l_id = vec_id.at(i);
                      auto t = vec_rpc_time.at(i);
                      auto t0 = vec_t0.at(i);
                      auto f1_calib = calib_functions.at(l_id);
                      if(!f1_calib) {
                        corrected_dt.push_back(t-t0);
                        continue;
                      }
                      auto w0 = vec_w0.at(i);
                      auto corr = f1_calib->Eval(w0);
                      auto corrected = t - t0 - corr;
                      corrected_dt.push_back(corrected);
                    }
                    return corrected_dt;
                  },
                  {"time", "TOF400Conteiner.fTimeT0", "w0", "linear_id"})

  ;
  std::vector< std::vector< ROOT::RDF::RResultPtr<::TH1D> > > h1_tot;
  std::vector<std::string> cuts;
  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string str_coordinate = "plane"+std::to_string(plane_id)+"_strip"+std::to_string( strip_id );
      auto l_id = plane_id * 48 + strip_id;
      dd = dd.Define( str_coordinate,
                      [l_id](ROOT::VecOps::RVec<int> linear ){
        ROOT::VecOps::RVec<int> passed_cuts;
        for( int i=0; i<linear.size(); ++i ){
          if( linear.at(i) != l_id ){
            passed_cuts.push_back(0);
            continue;
          }
          passed_cuts.push_back(1);
        }
        return passed_cuts;
        }, { "linear_id" });
      cuts.emplace_back( str_coordinate );
    }
  }

  for( int plane_id = 0; plane_id < 20; ++plane_id ){
    h2_tof_vs_tot.emplace_back();
    h1_tot.emplace_back();
    for( int strip_id = 0; strip_id < 48; ++strip_id ){
      std::string str_coordinate = "plane"+std::to_string(plane_id)+"_strip"+std::to_string( strip_id );
      h2_tof_vs_tot.back().push_back( dd.Histo2D({str_coordinate.c_str(), ";RPC width;dt", 140, 10, 80, 140, -2, 5}, "width", "dt", str_coordinate) );
      h1_tot.back().push_back( dd.Histo1D({std::data("width_"+str_coordinate), ";width;counts", 100, 0, 100}, "width", str_coordinate) );
    }
  }

  auto h2_tof_vs_tot_integral = dd.Histo2D({"tof_vs_tot_integral", ";RPC width;dt", 1024, 0, 100, 1024, -5, 25}, "width", "dt");

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
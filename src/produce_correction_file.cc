//
// Created by Misha on 5/26/2023.
//

#include <cassert>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <TFile.h>
#include <TF1.h>
#include <TGraph.h>

int main(int n_args, char** args ){
  assert( n_args > 3 );
  std::string str_t0_corr{ args[1] };
  std::string str_rpc_corr{ args[2] };
  std::string str_out_corr{ args[3] };
  auto file_t0 = std::unique_ptr<TFile, std::function<void(TFile*)> >( TFile::Open( str_t0_corr.c_str() ), []( TFile* file ){file->Close();});
  auto file_rpc = std::unique_ptr<TFile, std::function<void(TFile*)> >( TFile::Open( str_rpc_corr.c_str() ), []( TFile* file ){file->Close();});
  auto file_out = std::unique_ptr<TFile, std::function<void(TFile*)> >( TFile::Open( str_out_corr.c_str(), "RECREATE" ), []( TFile* file ){file->Close();});

  std::vector<int> planes(20, 0);
  std::iota(planes.begin(), planes.end(), 0);
  std::vector<int> strips(48, 0);
  std::iota(strips.begin(), strips.end(), 0);

  for( auto plane : planes ){
    for( auto strip : strips ){
      std::string directory = "plane_"+std::to_string(plane)+"/strip_"+std::to_string(strip)+"/";
      TGraph* g1_t0{nullptr};
      TF1* f1_t0{nullptr};
      TGraph* g1_rpc{nullptr};
      std::string graph_name = directory+"g1_tof_tot";
      std::string func_name = directory+"f1_tof_tot";
      file_t0->GetObject( graph_name.c_str(), g1_t0 );
      file_t0->GetObject( func_name.c_str(), f1_t0 );
      file_rpc->GetObject( graph_name.c_str(), g1_rpc );
      file_out->cd("/");
      std::string str_out_directory = "Plane_"+std::to_string(plane)+"/";
      file_out->mkdir( str_out_directory.c_str() );
      file_out->cd(str_out_directory.c_str());
      std::string str_out_g1_t0 = "gT0_TA_Plane"+std::to_string(plane)+"_Str"+std::to_string(strip);
      std::string str_out_f1_t0 = "T0_TA_Plane"+std::to_string(plane)+"_Str"+std::to_string(strip);
      std::string str_out_g1_rpc = "Rpc_TA_Plane"+std::to_string(plane)+"_Str"+std::to_string(strip);
      if( g1_t0 ) g1_t0->Write( str_out_g1_t0.c_str() );
      if( f1_t0 ) f1_t0->Write( str_out_f1_t0.c_str() );
      if( g1_rpc ) g1_rpc->Write( str_out_g1_rpc.c_str() );
    }
  }

  return 0;
}
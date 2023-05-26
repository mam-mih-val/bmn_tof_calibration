//
// Created by Misha on 3/30/2023.
//

#include <cassert>
#include <cstdlib>
#include <numeric>
#include <TCanvas.h>

#include "file_parser.h"
#include "fitter.h"

int main(int n_args, char** args){
  assert(n_args > 2);

  auto str_file_in = args[1];
  auto str_file_out = args[2];
  auto n_bins = atoi(args[3]);
  auto rebin_factor = atoi(args[4]);

  FileParser parser(str_file_in);
//  std::vector<int> planes{6};
  std::vector<int> planes(20, 0);
  std::iota( planes.begin(), planes.end(), 0 );
  std::vector<int> strips(48, 0);
  std::iota( strips.begin(), strips.end(), 0 );

  auto file_out = std::make_unique<TFile>( str_file_out, "recreate" );
  file_out->cd();
  auto pdf_name = std::string(str_file_out)+".pdf";
  auto canv = std::make_unique<TCanvas>();
  canv->Print( std::data( pdf_name+"[" ) );
  for( auto plane : planes ) {
    std::string str_plane = "plane_" + std::to_string(plane);
    file_out->mkdir(std::data(str_plane));
    file_out->cd(std::data(str_plane));
    for (auto strip : strips ) {
      auto h2 = parser.ReadHistogram(plane, strip);
      file_out->mkdir(std::data(str_plane + "/strip_" + std::to_string(strip)));
      file_out->cd(std::data(str_plane + "/strip_" + std::to_string(strip)));
      if( h2->Integral() < 1000 )
        continue;
      Fitter fitter(h2, [rebin_factor]( TH1D* h1_proj ){
        h1_proj->Rebin(rebin_factor);
        auto tof_min = h1_proj->GetXaxis()->GetXmin();
        auto tof_max = h1_proj->GetXaxis()->GetXmax();
        auto tof_range = fabs(tof_max - tof_min);
        std::string histo_name{ h1_proj->GetName() };
        auto f1_projection_fit = new TF1( histo_name.c_str(), "gaus", tof_min, tof_max );
        auto maximum = h1_proj->GetMaximum();
        auto std_dev = h1_proj->GetStdDev();
        auto maximum_bin = h1_proj->GetMaximumBin();
        auto maximum_tof = h1_proj->GetBinCenter(maximum_bin);
        auto first_filled_bin = h1_proj->FindFirstBinAbove( 1 );
        auto first_x = h1_proj->GetBinCenter(first_filled_bin);
        auto last_bin_of_interest = h1_proj->FindFirstBinAbove( maximum * 0.75 );
        auto last_x =  h1_proj->GetBinCenter(last_bin_of_interest);
        f1_projection_fit->SetParameter( 0, maximum );
        f1_projection_fit->SetParLimits( 0, 0, maximum );
//    f1_projection_fit->SetParameter( 0, maximum );
//    f1_projection_fit->SetParameter( 1, maximum_tof );
//    f1_projection_fit->SetParLimits( 1, tof_min, maximum_tof );
//    f1_projection_fit->SetParameter( 2, 0.1 );
        f1_projection_fit->SetParLimits( 2, 0.0, 0.15 );
//    f1_projection_fit->SetParameter( 3, 1.0 );
//    f1_projection_fit->SetParLimits( 3, 0.0, 5.0 );
        auto length = fabs(tof_max-tof_min);
        h1_proj->Fit( f1_projection_fit, "", "", first_x, last_x );
//    h1_proj->Fit( "pol4", "", "", maximum_tof - std_dev, maximum_tof + 4*std_dev );
        auto y = f1_projection_fit->GetParameter(1);
        return std::tuple{y, f1_projection_fit};
      }, [](TGraph* g1){ g1->Fit( "pol1" ); } );
      fitter.SetNPoints(n_bins);
      fitter.SetTofRebinFactor(rebin_factor);
      fitter.Fit();
      fitter.Dump();
      fitter.PlotResults( pdf_name, str_plane + "/strip_" + std::to_string(strip) );
      strip++;
      file_out->cd(std::data(str_plane));
    }
  }
  canv->Print( std::data( pdf_name+"]" ) );
  file_out->Close();
  return 0;
}
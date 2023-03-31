//
// Created by Misha on 3/30/2023.
//

#include "fitter.h"

void Fitter::CalculateBinEdges() {
  auto h1_x_proj = h2_histo_->ProjectionX();
  auto n_bins = h1_x_proj->GetNbinsX();
  h1_x_proj->Scale( 1.0 / h1_x_proj->Integral("width") );
  auto integral = h1_x_proj->Integral("width");
  double entries{0.0};
  double sum_wx_{0.0};
  int i=1;
  int j=1;
  bin_edges_.push_back(i);
  while( i < n_bins ){
    j=0;
    while( entries < integral / n_points_ && j+i < n_bins ){
      auto x = h1_x_proj->GetBinCenter(i+j);
      auto w = h1_x_proj->GetBinContent(i+j);
      auto dx = h1_x_proj->GetBinWidth(i+j);
      entries+=w*dx;
      sum_wx_+=w*x*dx;
      j++;
    }
    bin_edges_.push_back( i+j );
    bin_mean_.push_back( sum_wx_ / entries );
    entries=0;
    sum_wx_=0;
    i += j;
  }
}

void Fitter::Fit() {
  CalculateBinEdges();
  for( int i=0; i<bin_edges_.size()-1; ++i ){
    auto lo = bin_edges_.at(i);
    auto hi = bin_edges_.at(i+1);
    auto x = bin_mean_.at(i);
    std::string histo_name = std::to_string(i) + "-" + std::to_string(lo) + "-" + std::to_string(hi);
    auto h1_proj = h2_histo_->ProjectionY(histo_name.c_str(), lo, hi);
    h1_proj->Fit(width_distribution_formula_.c_str() );
    auto fit_function = dynamic_cast<TF1*>(h1_proj->GetListOfFunctions()->First());
    if( !fit_function )
      continue;
    auto y = fit_function->GetMaximumX();

    vector_x_.push_back(x);
    vector_y_.push_back(y);

    f1_fits_.push_back(fit_function);
    h1_y_slices_.push_back( h1_proj );
  }
  g1_tof_tot_ = new TGraph(vector_x_.size(), vector_x_.data(), vector_y_.data());
  g1_tof_tot_->Fit(tof_tot_formula_.c_str());
  f1_tof_tot_ = dynamic_cast<TF1*>(g1_tof_tot_->GetListOfFunctions()->First());
}

void Fitter::Dump() {
  assert(g1_tof_tot_);
  h2_histo_->Write();
  g1_tof_tot_->Write("g1_tof_tot");
  f1_tof_tot_->Write("f1_tof_tot");
  for( auto h1 : h1_y_slices_ ){
    h1->Write();
  }
}

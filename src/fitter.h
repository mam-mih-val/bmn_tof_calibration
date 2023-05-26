//
// Created by Misha on 3/30/2023.
//

#ifndef TOF400_CALIBRATION_FITTER_H
#define TOF400_CALIBRATION_FITTER_H


#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TList.h>
#include <TGraph.h>
#include <TCanvas.h>

#include "utils.h"

class NoRegularization{
public:
  NoRegularization() = default;
  ~NoRegularization() = default;
  void operator()(TGraph* g1){}
};

template<typename FitFunction, typename RegularizationFunction=NoRegularization>

class Fitter {
public:
  explicit Fitter(TH2D *h_2_histo,
                  const FitFunction& fit_function,
                  const RegularizationFunction& regularization_function=NoRegularization()) :
                  h2_histo_(h_2_histo),
                  fit_function_(fit_function),
                  regularization_function_(regularization_function) {}
  explicit Fitter(TH2D *h_2_histo,
                  FitFunction&& fit_function,
                  RegularizationFunction&& regularization_function=NoRegularization()) :
                  h2_histo_(h_2_histo),
                  fit_function_(std::move(fit_function)),
                  regularization_function_(std::move(regularization_function)) {}
  virtual ~Fitter() = default;
  void Dump(bool debug=false){
    assert(g1_tof_tot_);
    if(debug) h2_histo_->Write();
    g1_tof_tot_->Write("g1_tof_tot");
    if(f1_tof_tot_) f1_tof_tot_->Write("f1_tof_tot");
    if( !debug ) return;
    for( const auto& h1 : h1_y_slices_ ){
      h1->Write();
    }
  };
  void Fit(){
    CalculateBinEdges();
    for( int i=0; i<bin_edges_.size()-1; ++i ){
      auto lo = bin_edges_.at(i);
      auto hi = bin_edges_.at(i+1)-1;
      auto x = bin_mean_.at(i);
      std::string histo_name = std::to_string(i) + "-" + std::to_string(lo) + "-" + std::to_string(hi);
      auto h1_proj = std::unique_ptr<TH1D>(h2_histo_->ProjectionY(histo_name.c_str(), lo, hi));
//      if( h1_proj->Integral(1, h1_proj->GetNbinsX() ) < min_entries_ )
//        continue;
      h1_proj->Sumw2();
      auto [y, f1_fit] = fit_function_( h1_proj.get() );
      h1_y_slices_.emplace_back( std::move(h1_proj) );
      f1_fits_.push_back( std::unique_ptr<TF1>(f1_fit) );
      vector_x_.push_back(x);
      vector_y_.push_back(y);
    }
    g1_tof_tot_ = std::make_unique<TGraph>(vector_x_.size(), vector_x_.data(), vector_y_.data());
    regularization_function_(g1_tof_tot_.get());
    //    g1_tof_tot_->Fit(tof_tot_formula_.c_str());
    f1_tof_tot_ = dynamic_cast<TF1*>(g1_tof_tot_->GetListOfFunctions()->First());
  };

  [[nodiscard]] TH2D *GetH2Histo() const {
    return h2_histo_;
  }

  [[nodiscard]] TGraph *GetResultGraph() const {
    return g1_tof_tot_.get();
  }

  void SetNPoints(int n_points) {
    min_entries_ = n_points;
  }

  void SetTofTotFormula(const std::string &tof_tot_formula) {
    tof_tot_formula_ = tof_tot_formula;
  }

  void SetTofRebinFactor(int tof_rebin_factor){
    tof_rebin_factor_ = tof_rebin_factor;
  };

  void PlotResults(const std::string& str_out_file, const std::string& out_name) {
    auto canv = std::make_unique<TCanvas>( out_name.c_str(), out_name.c_str() );
    canv->cd();
    h2_histo_->Draw("colz");
    g1_tof_tot_->Draw("same");
    canv->Print( str_out_file.c_str() );
  };

private:
  void CalculateBinEdges(){
    auto n_bins = h2_histo_->GetNbinsX();
    std::string proj_name{"px_" + std::string(h2_histo_->GetName())};
    auto h1_x_proj = std::unique_ptr<TH1D>(h2_histo_->ProjectionX(proj_name.c_str(), 1, n_bins));
    double entries{0.0};
    double sum_wx{0.0};
    int i=1;
    bin_edges_.push_back(i);
    while( i < n_bins ){
      int j=0;
      while( entries < min_entries_ && j + i < n_bins ){
        auto x = h1_x_proj->GetBinCenter(i+j);
        auto w = h1_x_proj->GetBinContent(i+j);
        entries+=w;
        sum_wx+= w * x;
        j++;
      }
      bin_edges_.push_back( i+j );
      bin_mean_.push_back( sum_wx / entries );
      entries=0;
      sum_wx=0;
      i += j;
    }
  };

  FitFunction fit_function_;
  RegularizationFunction regularization_function_;
  TH2D* h2_histo_{nullptr}; // non-owning pointer to 2D histogram
  std::unique_ptr<TGraph> g1_tof_tot_{nullptr}; // Owning pointer to graph
  TF1* f1_tof_tot_{}; // Non-owning pointer to function
  int min_entries_{10};
  int tof_rebin_factor_{1};
  std::string tof_tot_formula_{"pol1"};
  std::vector<int> bin_edges_{};
  std::vector<double> bin_mean_{};
  std::vector<double> vector_x_{};
  std::vector<double> vector_y_{};
  std::vector<std::unique_ptr<TH1D>> h1_y_slices_{}; // Non-owning pointers to fit functions
  std::vector<std::unique_ptr<TF1>> f1_fits_{}; // Owning pointers to fit functions
};

#endif //TOF400_CALIBRATION_FITTER_H

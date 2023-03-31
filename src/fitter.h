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

class Fitter {
public:
  explicit Fitter(TH2D *h_2_histo) : h2_histo_(h_2_histo) {}
  virtual ~Fitter() = default;
  void CalculateBinEdges();
  void Dump();
  void Fit();

  [[nodiscard]] TH2D *GetH2Histo() const {
    return h2_histo_;
  }

  [[nodiscard]] TGraph *GetResultGraph() const {
    return g1_tof_tot_;
  }

  [[nodiscard]] const std::vector<TH1D *> &GetH1YSlices() const {
    return h1_y_slices_;
  }

  void SetNPoints(int n_points) {
    n_points_ = n_points;
  }

  void SetWidthDistributionFormula(const std::string &width_distribution_formula) {
    width_distribution_formula_ = width_distribution_formula;
  }

  void SetTofTotFormula(const std::string &tof_tot_formula) {
    tof_tot_formula_ = tof_tot_formula;
  }

private:
  TH2D* h2_histo_{nullptr};
  TGraph* g1_tof_tot_{nullptr};
  TF1* f1_tof_tot_{nullptr};
  int n_points_{10};
  std::string tof_tot_formula_{"pol1"};
  std::string width_distribution_formula_{"landau"};
  std::vector<int> bin_edges_{};
  std::vector<double> bin_mean_{};
  std::vector<double> vector_x_{};
  std::vector<double> vector_y_{};
  std::vector<TH1D*> h1_y_slices_{};
  std::vector<TF1*> f1_fits_{};
};


#endif //TOF400_CALIBRATION_FITTER_H

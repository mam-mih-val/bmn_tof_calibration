//
// Created by Misha on 3/30/2023.
//

#ifndef TOF400_CALIBRATION_FILE_PARSER_H
#define TOF400_CALIBRATION_FILE_PARSER_H


#include <memory>
#include <utility>

#include <TFile.h>
#include <TH2D.h>

class FileParser {
public:
  explicit FileParser(std::string file_name) : file_name_(std::move(file_name)), current_histogram_{} {
    file_in_ = std::make_unique<TFile>(file_name_.c_str());
  }
  virtual ~FileParser() = default;

  [[nodiscard]] TH2D* ReadHistogram(int plane, int strip) {
    std::string path="plane_"+std::to_string(plane)+"/plane"+std::to_string(plane)+"_strip"+std::to_string(strip);
    TH2D* h2{nullptr};
    file_in_->GetObject( path.c_str(), h2 );
    assert(h2);
    current_histogram_.reset(h2);
    return current_histogram_.get();
  }

private:
  std::string file_name_;
  std::unique_ptr<TFile> file_in_;
  std::unique_ptr<TH2D> current_histogram_;
};


#endif //TOF400_CALIBRATION_FILE_PARSER_H

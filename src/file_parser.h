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
  explicit FileParser(std::string file_name) : file_name_(std::move(file_name)) {
    file_in_ = std::make_unique<TFile>(file_name_.c_str());
  }
  virtual ~FileParser() = default;

  void ReadPlanes(std::vector<int> planes );

  [[nodiscard]] const std::vector<TH2D *> &GetReadHistograms(int plane) const {
    return read_histograms_.at(plane);
  }

private:
  std::string file_name_;
  std::unique_ptr<TFile> file_in_;
  std::map<int, std::vector<TH2D*>> read_histograms_;
};


#endif //TOF400_CALIBRATION_FILE_PARSER_H

//
// Created by Misha on 3/28/2023.
//

void converter(std::string file_in, std::string file_out="out.root"){
  ROOT::RDataFrame d( "TOF400", file_in );
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
          ;

  dd.Snapshot("tof400", file_out,dd.GetDefinedColumnNames());
}
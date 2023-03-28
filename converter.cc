//
// Created by Misha on 3/28/2023.
//

void converter(std::string file_in, std::string file_out="out.root"){
  ROOT::RDataFrame d( "TOF400", file_in );
  auto dd = d
          .Define("plane", "TOF400Conteiner.fPlane")
          .Define("strip", "TOF400Conteiner.fStrip")
          .Define("time", "TOF400Conteiner.fTime")
          .Define("width", "TOF400Conteiner.fWidth")
          .Define("T0", "TOF400Conteiner.fT0")
          .Define("T0_width", "TOF400Conteiner.fWidthT0")
          ;

  dd.Snapshot("t", file_out,dd.GetDefinedColumnNames());
}
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(TOF400-calibration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE debug)

find_package(ROOT REQUIRED)

set(SRC src/fitter.cc src/file_parser.cc src/utils.cc)

add_executable( fit_t0 src/fit_t0.cc ${SRC} )
target_link_libraries( fit_t0 ROOT::Tree ROOT::RIO ROOT::ProofBench )

add_executable( fit_rpc src/fit_rpc.cc ${SRC} )
target_link_libraries( fit_rpc ROOT::Tree ROOT::RIO ROOT::ProofBench )

add_executable( produce_correction_file src/produce_correction_file.cc )
target_link_libraries( produce_correction_file ROOT::Tree ROOT::RIO ROOT::ProofBench )
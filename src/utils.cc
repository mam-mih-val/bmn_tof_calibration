//
// Created by Misha on 4/3/2023.
//

#include <valarray>
#include "utils.h"

double Utils::SkewGauss(double x, double scale, double xi, double omega, double alpha) {
  const auto sub = ( x-xi ) / omega;
  const auto sub2 = sub*sub;
  const auto sqrt_1_2pi = 1.0 / sqrt( 2.0 * M_PI );
  auto rho = scale / ( omega * sqrt_1_2pi ) * exp( -sub2 )  * ( 1 + erf( - sub*alpha ) );
  return rho;
}

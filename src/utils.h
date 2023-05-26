//
// Created by Misha on 4/3/2023.
//

#ifndef TOF400_CALIBRATION_UTILS_H
#define TOF400_CALIBRATION_UTILS_H


namespace Utils {
   auto skew_gauss = [](double* xx, double* pp ){
    auto x = xx[0];
    auto scale = pp[0];
    auto xi = pp[1];
    auto omega = pp[2];
    auto alpha = pp[3];

    const auto sub = ( x-xi ) / omega;
    const auto sub2 = sub*sub;
    const auto sqrt_1_2pi = 1.0 / sqrt( 2.0 * M_PI );
    auto rho = scale / ( omega * sqrt_1_2pi ) * exp( -sub2 )  * ( 1 + erf( - sub*alpha ) );
    return rho;
  };
  double SkewGauss( double x, double scale, double xi, double omega, double alpha);
};


#endif //TOF400_CALIBRATION_UTILS_H

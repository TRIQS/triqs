#include "./det_manip.hpp"

namespace triqs { namespace det_manip {

complex_as_logphi& complex_as_logphi::operator=(std::complex<double> a) {
  log=std::log(std::abs(a));
  phi=std::arg(a);
  return *this;
}

complex_as_logphi& complex_as_logphi::operator*=(const complex_as_logphi& a) {
  log+=a.log;
  phi+=a.phi;
  return *this;
}

complex_as_logphi& complex_as_logphi::operator*=(std::complex<double> a) {
  log+=std::log(std::abs(a));
  phi+=std::arg(a);
  return *this;
}

complex_as_logphi::operator double() const{
  double a = std::exp(log);
  if (not std::isfinite(a)) TRIQS_RUNTIME_ERROR << "det_manip::determinant is underflowing!";
  //if ( (phi!=0.0) or (phi != PI) )
  //  TRIQS_RUNTIME_ERROR << "det_manip casting insto double truncates the imaginary part!";
  return a*real(std::exp(std::complex<double>(0.0,1.0)*phi));
}

complex_as_logphi::operator std::complex<double>() const {
  double a = std::exp(log);
  if (not std::isfinite(a)) TRIQS_RUNTIME_ERROR << "det_manip::determinant is underflowing!";
  return a*std::exp(std::complex<double>(0.0,1.0)*phi);
}

complex_as_logphi operator*(complex_as_logphi a, const complex_as_logphi& b) {
  a*=b; 
  return a;
}

complex_as_logphi operator*(complex_as_logphi a, std::complex<double> b) {
  a*=b;
  return a;
}

complex_as_logphi operator*(std::complex<double> b, complex_as_logphi a) {
  return a*b;
}

complex_as_logphi operator/(complex_as_logphi a, std::complex<double> b) {
  b = 1.0/b; 
  return a*b;
}

complex_as_logphi operator/(std::complex<double> b, complex_as_logphi a) {
  a.log *= -1.0;
  a.phi *= -1.0;
  return b*a;
}

}}

#pragma once
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
using uint = unsigned int;

template <typename TimeSeries> void boost_independent_gaussian_vector(TimeSeries& t, int seed) {
 boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)),
                                                                                  (boost::normal_distribution<>()));
 for (size_t i = 0; i < t.size(); ++i) t[i] = generator();
}

template <typename TimeSeries> void correlated_gaussian_vector(TimeSeries& t, int seed, size_t correlation_length) {
 boost_independent_gaussian_vector(t, seed);
 TimeSeries B(t.size());
 B[0] = t[0];
 double f = exp(-1. / correlation_length);
 for (size_t i = 1; i < t.size(); i++) B[i] = f * B[i - 1] + sqrt(1 - f * f) * t[i];
 t = B;
}


template <typename TimeSeries> void correlated_gaussian_vector(TimeSeries& t, int seed, size_t correlation_length, double avg) {
 boost_independent_gaussian_vector(t, seed);
 TimeSeries B(t.size());
 B[0] = t[0];
 double f = exp(-1. / correlation_length);
 for (size_t i = 1; i < t.size(); i++) B[i] = f * B[i - 1] + sqrt(1 - f * f) * t[i];
 t = B;
 for (size_t i = 1; i < t.size(); i++) t[i] = t[i] + avg;
}


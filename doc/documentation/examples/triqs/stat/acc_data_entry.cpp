#include <triqs/stat/accumulator.hpp>
#include <triqs/arrays.hpp>

using namespace triqs::stat;
using namespace triqs::arrays;

int main() {
    // Accumulating Simple Scalars:
    accumulator<double> my_accumulator_d(0.0, 8, 100, 1);
    double my_measurement_d = 1.0;
    my_accumulator_d << my_measurement_d;

    // Accumulating Arrays:
    array<double, 2> my_array_instance;
    my_array_instance.resize(2,3);
    accumulator<array<double, 2>> my_accumulator_a(my_array_instance, 8, 100, 1); 
    // Passing an array of the same type and shape as the one definiting the accumulator
    array<double, 2> my_measurement_a{{0.0, 1.0, 2.0}, {3.0, 4.0, 5.0}};
    my_accumulator_a << my_measurement_a; 
    //FIXME: Add view


    // Automatic Compression on Linear Bins
}
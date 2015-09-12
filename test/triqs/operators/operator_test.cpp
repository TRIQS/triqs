/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by I. Krivenko
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include <triqs/utility/first_include.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <iostream>
#include <vector>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <sstream>

using namespace triqs::operators;
using namespace triqs;
using namespace triqs::hilbert_space;

int main(int argc, char **argv)
{
 try {
    // Operators without indices
    auto op_with_no_indices = c() + c_dag() - n();
    std::cout << "op_with_no_indices = " << op_with_no_indices << std::endl;

    // Operators with many indices
    auto op_with_many_indices = c(1,2,"a",true,-2) +
                            c_dag(3,15,"b",false,-5);
    std::cout << "op_with_many_indices = " << op_with_many_indices << std::endl;

    // Constant operator
    many_body_operator const_op(3.14);
    std::cout << "const_op = " << const_op << std::endl;

    // Commutation relations
    std::vector<many_body_operator > C = {c(1), c(2), c(3)};
    std::vector<many_body_operator > Cd = {c_dag(1), c_dag(2), c_dag(3)};

    std::cout << std::endl << "Anticommutators:" << std::endl;
    for(auto const& cdi : Cd)
    for(auto const& ci : C){
        std::cout << "{" << cdi << ", " << ci << "} = " << cdi*ci + ci*cdi << std::endl;
    }

    std::cout << std::endl << "Commutators:" << std::endl;
    for(auto const& cdi : Cd)
    for(auto const& ci : C){
        std::cout << "[" << cdi << ", " << ci << "] = " << cdi*ci - ci*cdi << std::endl;
    }

    // Algebra
    auto x = c(0);
    auto y = c_dag(1);

    std::cout << std::endl << "Algebra:" << std::endl;    
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;

    std::cout << "-x = " << -x << std::endl;
    std::cout << "x + 2.0 = " << x + 2.0 << std::endl;
    std::cout << "2.0 + x = " << 2.0 + x << std::endl;
    std::cout << "x - 2.0 = " << x - 2.0 << std::endl;
    std::cout << "2.0 - x = " << 2.0 - x << std::endl;
    std::cout << "3.0*y = " << 3.0*y << std::endl;
    std::cout << "y*3.0 = " << y*3.0 << std::endl;
    std::cout << "x + y = " << x + y << std::endl;
    std::cout << "x - y = " << x - y << std::endl;
    std::cout << "(x + y)*(x - y) = " << (x + y)*(x - y) << std::endl;

    // N^3
    std::cout << std::endl << "N^3:" << std::endl;
    auto N = n("up") + n("dn");
    auto N3 = N*N*N;
    std::cout << "N = " << N << std::endl;
    std::cout << "N^3 = " << N3 << std::endl; 

/*    // Serialization
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa & N3;

    boost::archive::text_iarchive ia(ss);
    many_body_operator new_N3;
    ia & new_N3;
    
    std::cout << "New N^3 = " << new_N3 << std::endl; 
*/   
    auto X = c_dag(1) * c_dag(2) * c(3) * c(4);
    std::cout  << "X = "<< X<<std::endl; 
    std::cout  << "dagger(X) = "<< dagger(X)<<std::endl; 


    auto op1 = 2* c_dag(1) * c_dag(2) * c(3) * c(4)  + 3.4 * c_dag(0,"a") *  c(0,"a");

    auto f = h5::file("ess.h5", 'w');
    h5_write(f, "OP", op1);

    auto op2 = h5::h5_read<many_body_operator>(f, "OP");

    std::cerr << op1 << std::endl;
    std::cerr << op2 << std::endl;

    std::cerr << (op1 - op2).is_zero() << std::endl;


    // write operator op1 with imposed fundamental operator set
    fundamental_operator_set fs;
    fs.insert(6);
    for(int i=1; i<5; i++) fs.insert(i);
    fs.insert(0,"a");
    fs.insert(1,"a");
    h5_write(f, "OP_fs", op1, fs);

    // read both operator and fundamental operator set
    fundamental_operator_set fs2;
    many_body_operator op3;
    h5_read(f, "OP_fs", op3, fs2);
    std::cerr << (op1 - op3).is_zero() << std::endl;

    return 0;
 }
 catch(std::exception const & e) {
  std::cout << e.what() << std::endl;
 }
}

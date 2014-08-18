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
#include <iostream>
#include <pytriqs/converters/operators2.hpp>
#include <triqs/parameters.hpp>
#include <triqs/operators/many_body_operator.hpp>

using namespace triqs::utility;
using namespace triqs::params;

int main(int argc, char **argv) {
    
    // Operators with many indices
    auto op_with_many_indices = c(1,2,"a",true,-2) + c_dag(3,15,"b",false,-5);
    std::cout << "op_with_many_indices = " << op_with_many_indices << std::endl;
                            
    parameters p{};
    p.add_field("hamilt", no_default<many_body_operator<double>>(), "Number of QMC cycles");

    p["hamilt"] = op_with_many_indices;

    auto x = p["hamilt"];

    std::cout << "test = " << x << std::endl;
}

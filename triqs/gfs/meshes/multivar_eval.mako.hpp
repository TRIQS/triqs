<%
# List all policies
interpols = {1 : "interpol_t::None", 2 : "interpol_t::Linear1d"}

# List all cases for which to generate the code
to_be_generated= [ (1,1),(1,1,1), (1,1,1,1), 
           (1,2), (2,1), (2,2) ]

def generate(*ns):
  import itertools
  def one_term(ps) : 
     coefs, indices = [], []
     for i,p in enumerate(ps):
        if ns[i] > 1 : 
	   coefs.append("id%s.w%s"%(i,p))
	   indices.append("id%s.i%s"%(i,p))
	else: 
	   indices.append("id%s"%i)
     coefs = " * ".join(coefs)
     return coefs + (" *" if coefs else '') +  " g[{"  + ', '.join(indices) + "}]"
  return [one_term(ps) for ps in itertools.product(*[range(n) for n in ns])]

def gen_enumerate(n, code1, code2 = '', sep=', ') : 
  return sep.join(code1 + str(i) + ((code2 + str(i)) if code2 else '') for i in range(n))
%>  

<%def name ="gen(*ns)">
  template <> struct multivar_eval<${', '.join(interpols[n] for n in ns)}> {
   template <typename G, ${gen_enumerate(len(ns),"typename A")}> 
    auto operator()(G const &g, ${gen_enumerate(len(ns), "A", " && a")}) const {
   %for i,n in enumerate(ns) :
    auto id${i} = std::get<${i}>(g.mesh().components()).get_interpolation_data(${interpols[n]}{},a${i});
   %endfor 
    return 
${' +\n'.join(generate(*ns))};
   }
  };
</%def>

/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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
#pragma once
namespace triqs {
namespace gfs {
 
 template <typename ... T> struct multivar_eval;

%for ps in to_be_generated : 
   ${gen(*ps)} 
 %endfor

}}  

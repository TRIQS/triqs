
from cpp2py.compiler import compile

code = """
   #include <triqs/gfs.hpp>
   
  int f (int x) { return x+3;}
   
  using namespace triqs::gfs;

  gf<imfreq> ma(int n) { return {{10,Fermion, n}, {2,2}};}
"""

M = compile(code, modules = "pytriqs", cxxflags = '-O2 ')


print M.f(2)
g = M.ma(100)


if 1: 
    # 
    code = """        
    #include <vector>

    std::vector<int> f(int x) { return {x,x+1};}
    //std::vector<double> g(int x) { return {1.0*x,x+2.0};}
    """

    m= compile(code, verbosity =3, recompile = False)

    assert m.f(1) == [1, 2]


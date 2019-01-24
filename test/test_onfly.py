import cpp2py.compiler as M

if 1: 
    # 
    code = """        
    #include <vector>
    std::vector<int> f(int x) { return {x,x+1};}
    //std::vector<double> g(int x) { return {1.0*x,x+2.0};}
    """

    m= M.compile(code, verbosity =3)

    assert m.f(1) == [1, 2]
    #assert m.g(1) == [1, 3]

if 0:
    # 
    code = """   
    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    dcomplex f(gf_view<imfreq> g) { return g[0](0,0); }
    """

    m2= M.compile(code, verbosity = 3, modules = ['pytriqs'])

    from pytriqs.gf import *
    gw = GfImFreq(indices = [1], beta = 50)

    print m2.f(gw)

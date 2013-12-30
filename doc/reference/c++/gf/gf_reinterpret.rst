.. highlight:: c

.. _gf_reinterpret:

Target reinterpretation
=========================

**Synopsis** :: 

    gf_view<Variable, matrix_valued, Opt> 
       reinterpret_scalar_valued_gf_as_matrix_valued(gf_view<Variable, scalar_valued, Opt>);
    
    gf_view<Variable, matrix_valued, Opt> 
       reinterpret_scalar_valued_gf_as_matrix_valued(gf<Variable, scalar_valued, Opt> &);

    gf_const view<Variable, matrix_valued, Opt> 
       reinterpret_scalar_valued_gf_as_matrix_valued(gf_const_view<Variable, scalar_valued, Opt>);
    
    gf_const_view<Variable, matrix_valued, Opt> 
       reinterpret_scalar_valued_gf_as_matrix_valued(gf<Variable, scalar_valued, Opt> const &);


Given a gf or a view, scalar_valued, it returns a view of a 1x1 matrix_valued function on the same data.


.. highlight:: c

.. _gf_eval_curry: 

Partial evaluation and Currying 
================================

.. warning::

   This part of the library is still in alpha, in development.
 
Multivariable Green function are be both partially evalued, and "curried".

Partial evaluation 
--------------------------

**Purpose** :

  Having a function (x,y) -> f(x,y), fix x = x0 in order to obtain a function of y only.

**Synopsis** ::

  template<int ... pos, typename Opt, typename Target, bool B, bool IsConst, typename IT, typename ... Ms>
  gf_view< sliced_domain_t<cartesian_product<Ms...>>  ,Target, Opt, IsConst>
   partial_eval(gf_impl< cartesian_product<Ms...>, Target,Opt,B,IsConst> const & g, IT index) { 
 
Probably an example would help here ... :

.. compileblock::
      
    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; using triqs::clef::placeholder;
    int main(){
     double beta =1, tmin=0, tmax=1.0;
     int n_re_time=100, n_im_time=100;
 
     using g_t_tau_s =  gf<cartesian_product<retime,imtime>, scalar_valued>;
     
     // a scalar valued function
     auto g= g_t_tau_s { { {tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}} };
      
     // evaluation of the second variable to 3 : 3 is the **index** of the point ! 
     auto g_sliced = partial_eval<1>(g(), 3); 
     // g_sliced is now a gf_view/gf_const_view<retime> seeing the value at the index 3 

     std::cout<< g_sliced<< std::endl;
    }


Currying
-------------

**Purpose** :

  Having a function (x,y) -> f(x,y), rewrite it as x-> y-> f(x,y)

**Synopsis** ::

  template <int... pos, typename Target, typename Opt, bool IsConst, typename... Ms>
  gf_view<cart_prod<triqs::tuple::filter_t<std::tuple<Ms...>, pos...>>,
                lambda_valued<curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target, Opt,IsConst>, pos...>>, Opt, IsConst>
  curry(gf_view<cartesian_product<Ms...>, Target, Opt, IsConst> g);
 
 
.. compileblock::
      
    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; using triqs::clef::placeholder;
    int main(){
     double beta =1, wmin=0, wmax=1.0;
     int n_re_freq=100, n_im_freq=100;
 
     using g_w_wn_s =  gf<cartesian_product<refreq,imfreq>, scalar_valued>;
     
     // a scalar valued function
     auto g= g_w_wn_s{ { {wmin, wmax, n_re_freq}, {beta, Fermion, n_im_freq}} };
 
     // put expression in it
     triqs::clef::placeholder<0> w_;
     triqs::clef::placeholder<1> wn_;
     g (w_,wn_)<<1/(wn_-1)/( w_ + 3_j );
 
     // Currying by selecting the first variable, i.e. t -> tau -> g(t,tau)
     auto g1 = curry<0>(g);

     // Currying by selecting the second variable, i.e. tau -> t -> g(t,tau)
     auto g2 = curry<1>(g);

     //std::cout << g << g1[1]<< g2 [2] << std::endl;
     std::cout<< g1[1][2] << g2 [2][1]  << g [{1,2}] << std::endl;
    }


 




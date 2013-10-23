.. highlight:: c

.. _gf_block: 

block_gf<G> (alias of gf<block_index, G>) 
===================================================

This is a specialisation of :ref:`gf_and_view` for block functions.

A block Green function is nothing but Green function on a discrete domain representing the 
block indices, and whose value is a Green function itself.

For convenience, for following aliases are provided ::
    
   template<typename ... T> using block_gf            = gf           <block_index, gf<T...>>;
   template<typename ... T> using block_gf_view       = gf_view      <block_index, gf<T...>>;
   template<typename ... T> using block_gf_const_view = gf_const_view<block_index, gf<T...>>;

Constructors
--------------

The constructors of the gf.

target_shape_t = {}


Factories
-------------

Also several factories are provided for these block Green functions.

Factories for the regular type
.....................................

The factories for the regular type *gf* are ::

 // From a number and a gf to be copied
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> 
 make_block_gf(int n, gf<Variable, Target, Opt> const & g);

 // From a vector of gf (moving directly)
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> 
 make_block_gf(std::vector<gf<Variable, Target, Opt>> V);

 // From a vector of gf or views (a gf must be contructible from GF2)
 template <typename Variable, typename Target, typename Opt, typename GF2>
 block_gf<Variable, Target, Opt> 
 make_block_gf(std::vector<GF2> const &V);

 // From a init list of GF with the correct type
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> 
 make_block_gf(std::initializer_list<gf<Variable, Target, Opt>> const &V);

 // From vector<string> and a gf to be copied
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> 
 make_block_gf(std::vector<std::string> block_names, gf<Variable, Target, Opt> const &g);
 
 // From vector<string>, vector<gf>
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> 
 make_block_gf(std::vector<std::string> block_names, std::vector<gf<Variable, Target, Opt>> V);

 // From vector<string>, init_list<GF>
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> 
 make_block_gf(std::vector<std::string> block_names,
               std::initializer_list<gf<Variable, Target, Opt>> const &V);


Factories for the view type
.....................................

The factories for the view type *gf_view*, *gf_const_view* are ::

 // Build a view of the gf. 
 // 1- All Green functions must have the same type
 // 2- [Expert only] This is a simplified version of the code. 
 //    In the real code, the reference is of stripped from G in the type deduction.
 template<typename ... G> 
  gf_view<block_index, G> 
  make_block_gf_view(G && ... g);

 // Make a view from a vector of gf
 template<typename GF>
  gf_view<block_index,GF> 
  make_block_gf_view_from_vector (std::vector<GF> V);


Domain & mesh
----------------

The domain is a discrete domain ...

Singularity
-------------

None.

Interpolation method
---------------------

None.

Data storage
---------------

Not exposed to user.

HDF5 storage convention
---------------------------

TO BE WRITTEN.

Iterators
-----------

TO BE COMPLETED.

Block gf have a natural iterator, e.g. ::

 // if a BG is block function
 for (auto & g : BG) ....



Examples
---------
.. compileblock::

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs; using triqs::clef::placeholder;
    int main() {
     double beta =1; 

     // Make a block gf of 3 gf, which are empty (default constructed). 
     auto Bg0 = block_gf<imfreq> (3); 

     // make a few gf<imfreq> ...
     auto g1 = gf<imfreq> ({beta, Fermion}, {2,2});

     // ... and construct some block function out of them.
     auto Bg1 = make_block_gf<imfreq> (3, g1); 
     auto Bg2 = make_block_gf<imfreq> ({g1,g1,g1});
     auto Bg3 = make_block_gf<imfreq> ({"a","b","c"}, {g1,g1,g1}); 

     // do something on all blocks
     for (auto const &g : Bg1) {
       std::cout<< g.mesh() << std::endl;
     }

     // multiply them by 1,2,3  ...
     auto i = 1;
     for (auto & g : Bg1) g = g * i++;

     // a little save in an hdf5 file ?
     H5::H5File file("test_block_gf.h5", H5F_ACC_TRUNC );
     h5_write(file, "B3", Bg3);
    }

 

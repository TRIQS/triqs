/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_GF_BLOCK_H
#define TRIQS_GF_BLOCK_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./meshes/discrete.hpp"

namespace triqs { namespace gfs {

 struct block_index {};

 namespace gfs_implementation { 

  template<typename Opt> struct mesh<block_index,Opt>                { typedef discrete_mesh<discrete_domain> type;};
  template<typename Target, typename Opt> struct h5_name<block_index,Target,Opt>      { static std::string invoke(){ return  "BlockGf";}};

  /// ---------------------------  h5_rw ---------------------------------

  template <typename Target,typename Opt> struct h5_ops<block_index,Target,Opt> { 

   template<typename DataType, typename GF> 
    static void write(h5::group g, std::string const & s, DataType const & data, GF const & gf) {
     auto gr =  g.create_group(s);
     for (size_t i =0; i<gf.mesh().size(); ++i) h5_write(gr,gf.mesh().domain().names()[i],data[i]);
    }

   template<typename DataType,typename GF> 
    static void read(h5::group g, std::string const & s, DataType & data, GF const & gf) {
     auto gr =  g.create_group(s);
     for (size_t i =0; i<gf.mesh().size(); ++i) h5_read(gr,gf.mesh().domain().names()[i],data[i]);
    }
  };

  /// ---------------------------  evaluator ---------------------------------


  /// ---------------------------  data access  ---------------------------------

  template<typename Target, typename Opt> struct data_proxy<block_index,Target,Opt> : data_proxy_vector <typename non_view_type_if_exists_else_type<Target>::type>{};

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Target, typename Opt>
   struct factories<block_index,Target,Opt>  { 
    typedef typename mesh<block_index, Opt>::type mesh_t;
    typedef gf<block_index,Target> gf_t;
    typedef gf_view<block_index,Target> gf_view_t;

    static gf_t make_gf(std::vector<Target> const & V)  { return gf_t ( mesh_t(V.size()), V,            nothing(), nothing() ) ; }
    static gf_t make_gf(std::vector<Target> && V)       { return gf_t ( mesh_t(V.size()), std::move(V), nothing(), nothing() ) ; }

    static gf_t make_gf(std::vector<std::string> const & block_names, std::vector<Target> const & V) {
     return gf_t(mesh_t(block_names), V, nothing(), nothing() );
    }
    static gf_t make_gf(std::vector<std::string> const & block_names, std::vector<Target> && V) {
     return gf_t(mesh_t(block_names), std::move(V), nothing(), nothing() );
    }

   /* static gf_t make_gf(std::initializer_list<Target> const & l) { 
      auto v = std::vector<Target> {l};
      return make_gf(v);
    }
*/
    /* template<typename... Args>
       static gf_t make_gf(size_t N, Args&& ...args)  {
       std::vector<Target> V; V.reserve(N);
       for (size_t i=0; i<N; ++i) V.push_back( Target::make_gf (std::forward<Args>(args...)));
       return make_gf(V);
       }
       */
    static gf_t make_gf(int N, Target const & g)  {
     std::vector<Target> V; V.reserve(N);
     for (size_t i=0; i<N; ++i)  V.push_back(g);
     return make_gf(V);
    }

    static gf_t make_gf(std::vector<std::string> const & block_names, Target const & g)  {
     std::vector<Target> V; V.reserve(block_names.size());
     for (size_t i=0; i<block_names.size(); ++i)  V.push_back(g);
     return make_gf(block_names,V);
    }

    /*  template<typename... Args>
	static gf_t make_gf(std::vector<std::string> const & block_names, Args&& ...args)  {
	std::vector<Target> V; V.reserve(block_names.size());
	for (size_t i=0; i<block_names.size(); ++i)  V.push_back( Target::make_gf (std::forward<Args>(args...)));
	return make_gf(block_names,V);
	}
	*/

    template<typename GF>
     static gf_view_t make_gf_view(std::vector<GF> const & V) { return gf_view_t ( mesh_t(V.size()), V,            nothing(), nothing() ) ; }
    template<typename GF>
     static gf_view_t make_gf_view(std::vector<GF> && V)      { return gf_view_t ( mesh_t(V.size()), std::move(V), nothing(), nothing() ) ; }

   };
 
 } // gfs_implementation

 // -------------------------------   Free function   --------------------------------------------------

 // a simple function to get the number of blocks
 template<typename T> size_t n_blocks (gf<block_index,T> const & g)      { return g.mesh().size();}
 template<typename T> size_t n_blocks (gf_view<block_index,T> const & g) { return g.mesh().size();}


 // template alias
 //template<typename T> using block_gf = gf<block_index, gf<T>>;

 // experimental
 template<typename Target,  typename ... U>
  gf<block_index, gf<Target>> make_block_gf(U && ...u) { return gfs_implementation::factories<block_index,gf<Target>,void>::make_gf(std::forward<U>(u)...);}

 // also experimental
 // an iterator over the block
  template<typename Target, typename Opt>
  class block_gf_iterator : 
   public boost::iterator_facade< block_gf_iterator<Target,Opt>, typename Target::view_type , boost::forward_traversal_tag, typename Target::view_type  > {
    friend class boost::iterator_core_access;
    typedef gf_view<block_index,Target,Opt> big_gf_t;
    typedef typename big_gf_t::mesh_t::iterator mesh_iterator_t;
    big_gf_t big_gf;
    mesh_iterator_t mesh_it;

    typename Target::view_type const & dereference() const { return big_gf(*mesh_it);}
    bool equal(block_gf_iterator const & other) const { return ((mesh_it == other.mesh_it));}
    public:
    block_gf_iterator(gf_view<block_index,Target,Opt> bgf, bool at_end = false): big_gf(std::move(bgf)), mesh_it(&big_gf.mesh(),at_end) {}
    void increment() { ++mesh_it; }
    bool at_end() const { return mesh_it.at_end();}
   };
 
  template<typename Target, typename Opt, bool B>
  block_gf_iterator<Target,Opt> begin(gf_impl<block_index,Target,Opt,B> const & bgf) { return {bgf,false};}
  
  template<typename Target, typename Opt, bool B>
  block_gf_iterator<Target,Opt> end(gf_impl<block_index,Target,Opt,B> const & bgf) { return {bgf,true};}




}}
#endif



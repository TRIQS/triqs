#pragma once
#include <triqs/arrays.hpp>
#include <boost/serialization/access.hpp>
namespace triqs { namespace arrays {
 
 ///implementation of a matrix by blocks
 /** implementation of a matrix by blocks **with named blocks**
   @tparam T scalar type (double/dcomplex)
   @note does not support matrix operations at the moment
  */
 template<typename T>
 struct block_matrix {

  std::vector<std::string> block_names;
  std::vector<matrix<T>> matrix_vec;
  int size() const { return matrix_vec.size();}

  block_matrix(std::vector<std::string> const & block_names_, std::vector<matrix<T>> const & matrix_vec_) : block_names(block_names_), matrix_vec(matrix_vec_){}
  block_matrix() : block_names(), matrix_vec(){}

  ///call operator with a string (slow)
  matrix_view<T> operator()(std::string s){
   int i=0;
   for(auto const & x : block_names) {
    if(s==x) break;
    i++;
   }
   if (i==block_names.size()) TRIQS_RUNTIME_ERROR << "block name " << s << " is incorrect" ;
   return matrix_vec[i];
  }
  ///subscript operator (fast)
  matrix<T> & operator[](int i) {
   return matrix_vec[i];
  }
  ///subscript operator (fast)
  matrix_const_view <T> operator[](int i) const {
   return matrix_vec[i];
  }
  friend std::string get_triqs_hdf5_data_scheme(block_matrix const &g) {
   return "BlockMatrix";
  }
  friend std::ostream & operator<< (std::ostream & out, block_matrix const & c) { 
   for(int i=0;i<c.block_names.size();i++)
    out <<c.block_names[i] << ": "<<c.matrix_vec[i] << std::endl;
   return out;
  }

  friend block_matrix mpi_all_reduce(block_matrix & m, mpi::communicator c={}, int root=0 ){
   block_matrix m_tot = m;
   for(int i=0;i<m.size();i++){
     m_tot[i] = mpi::mpi_all_reduce(m[i], c, root);
   }
   return m_tot;
  }
  friend void mpi_broadcast(block_matrix &m, mpi::communicator c = {}, int root = 0) {
   mpi_broadcast(m.block_names, c, root);
   mpi_broadcast(m.matrix_vec, c, root);
  }

  friend class boost::serialization::access;
  template<class Archive>
   void serialize(Archive & ar, const unsigned int version){
    ar & block_names;
    ar & matrix_vec;
   }

 };
 ///write to HDF5
 template<typename T>
 void h5_write (triqs::h5::group fg, std::string subgroup_name, block_matrix<T> const & c) {
  triqs::h5::group gr = fg.create_group(subgroup_name);
  h5_write(gr,"block_names",c.block_names);
  h5_write(gr,"matrix_vec",c.matrix_vec);
  h5_write_attribute(gr, "TRIQS_HDF5_data_scheme", get_triqs_hdf5_data_scheme(c));
 }
 ///read from HDF5
 template<typename T>
 void h5_read(triqs::h5::group fg, std::string subgroup_name, block_matrix<T> & c){
  triqs::h5::group gr = fg.open_group(subgroup_name);
  std::vector<std::string> block_names;
  std::vector<matrix<T>> matrix_vec;

  h5_read(gr,"block_names",block_names);
  h5_read(gr,"matrix_vec",matrix_vec);
  c = block_matrix<T>(block_names, matrix_vec);
}

}}

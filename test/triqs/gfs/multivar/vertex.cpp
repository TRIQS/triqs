#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 

using namespace triqs::gfs;
using  triqs::clef::placeholder;
namespace h5 = triqs::h5;

int main() {

 // scalar valued gf_vertex
 using gf_vertex_t        = gf<cartesian_product<imfreq,imfreq,imfreq>, scalar_valued>;
 using gf_vertex_tensor_t = gf<cartesian_product<imfreq,imfreq,imfreq>, tensor_valued<3>>;

 try { 

  double beta =10.0;
  int n_im_freq=10;

  auto m = gf_mesh<imfreq> {beta, Fermion, n_im_freq};

  auto vertex = gf_vertex_t { {m,m,m} };
  auto vertex2 = gf_vertex_t{}; //vertex;
 
  placeholder<0> w0_;
  placeholder<1> w1_;
  placeholder<2> w2_;
  
  vertex (w0_, w1_, w2_) << w0_ + 2.3*w1_ + 3.1*w2_;
  
  vertex [{0,0,0}]  = 10;

  std::cout  << " vertex [{1,6,3}] " << vertex [{1,6,3}] << std::acos(-1)*(2*1 +1)/10.0  + 2.3*std::acos(-1)*(2*6 +1)/10.0 + 3.1*std::acos(-1)*(2*3 +1)/10.0 << std::endl ;
  auto v = on_mesh(vertex);
  v(0,0,0) *=2;

  std::cout << vertex(0,0,0)<< std::endl;
  
  //saving 
  {
   h5::file file("vertex1.h5", H5F_ACC_TRUNC );
   h5_write(file, "v", vertex);
  }

  // loading
  {
   h5::file file("vertex1.h5", H5F_ACC_RDONLY );
   h5_read(file, "v", vertex2);
  }

  //resaving 
  {
   h5::file file("vertex1b.h5", H5F_ACC_TRUNC );
   h5_write(file, "v", vertex2);
  }

  // now with indices
  auto vertex3 = gf_vertex_tensor_t { {m,m,m} , {2,2,2} };
  auto vertex3b = vertex3;

  vertex3 [{0,0,0}](0,0,0)  = 10;
  std::cout << vertex3(0,0,0)<< std::endl;

  //saving 
  {
   h5::file file("vertex3.h5", H5F_ACC_TRUNC );
   h5_write(file, "v", vertex3);
  }

  // loading
  {
   h5::file file("vertex3.h5", H5F_ACC_RDONLY );
   h5_read(file, "v", vertex3b);
  }

  //resaving 
  {
   h5::file file("vertex3b.h5", H5F_ACC_TRUNC );
   h5_write(file, "v", vertex3b);
  }


 }
 TRIQS_CATCH_AND_ABORT;
}

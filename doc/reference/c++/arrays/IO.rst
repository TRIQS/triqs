.. highlight:: c

MPI and serialization  
##########################################

Serialization 
============================

The `value classes` and the views are compatible with Boost Serialization library.

MPI
============================

The `value classes` (array, matrix, vector) can be bcasted, reduced, 
with the boost.mpi library, e.g. :

.. compileblock::

     #include <triqs/arrays.hpp>
     #include <boost/mpi.hpp>
     using triqs::arrays::array;using triqs::clef::placeholder;

     int main() {
      boost::mpi::environment env;//argc, argv);
      boost::mpi::communicator world;

      array<long,2> A (2,2), B(2,2),C(2,2);
      placeholder<0> i_; placeholder<1> j_;
      A(i_,j_) << (1+world.rank())*(10*i_+ j_);

      if (world.rank() ==0) std::cout<<" A = "<<A<<std::endl;
      
      boost::mpi::reduce (world, A,C, std::plus<array<long,2> >(),0);

      int s= world.size();
      if (world.rank() ==0) std::cout<<" C = "<<C<< "  should be "<< array<long,2>( (s*(s+1)/2) * A) <<std::endl;
      }
  


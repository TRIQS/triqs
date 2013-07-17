.. highlight:: c

MPI and serialization  
##########################################

Serialization 
============================

The `value classes` and the views are compatible with Boost Serialization library.

MPI
============================

The `value classes` (array, matrix, vector) can be bcasted, reduced, 
with the boost.mpi library.

  Example::

   #include <<array.hpp>
   #include <boost/mpi.hpp>
   #include <iostream>
   using namespace triqs::arrays;
   namespace mpi=boost::mpi;

   int main() {
    mpi::environment env(argc, argv);
    mpi::communicator world;

    array<long,2> A (2,2), B(2,2),C(2,2);
    for (int i =0; i<2; ++i)
     for (int j=0; j<2; ++j) 
      { A(i,j) = (1+world.rank())*(10*i+ j);}

    if (world.rank() ==0) cout<<" A = "<<A<<endl;
    
    boost::mpi::reduce (world, A,C, std::plus<array<long,2> >(),0);

    int s= world.size();
    if (world.rank() ==0) cout<<" C = "<<C<< "  should be "<< array<long,2>( (s*(s+1)/2) * A) <<endl;
    
    return 0;
    }
  


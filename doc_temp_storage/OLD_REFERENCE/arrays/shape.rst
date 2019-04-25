.. _Shape:

Shape & dimensions
==================================

**Synopsis**:

.. code-block:: c

    template <ImmutableArray A> mini_vector<size_t,R> get_shape(A const &); // R is the rank of A 
    
    template<ImmutableArray A> size_t first_dim   (A const & x); 
    template<ImmutableArray A> size_t second_dim  (A const & x);
    template<ImmutableArray A> size_t third_dim   (A const & x);
    template<ImmutableArray A> size_t fourth_dim  (A const & x);
    template<ImmutableArray A> size_t fifth_dim   (A const & x);
    template<ImmutableArray A> size_t sixth_dim   (A const & x);
    template<ImmutableArray A> size_t seventh_dim (A const & x);


The shape and dimensions of any object modeling :ref:`ImmutableCuboidArray` is obtained using get_shape and xxx_dim functions :

.. triqs_example:: ./shape_0.cpp

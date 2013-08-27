.. highlight:: c
 
.. _arr_stream:

operator << 
==================================

**Synopsis**::

  template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0> 
  std::ostream & operator << (std::ostream & out, array_view<ValueType,Rank,OptionsFlags,TraversalOrder> const &);
   
  template <typename ValueType, ull_t OptionsFlags=0, ull_t TraversalOrder=0> 
  std::ostream & operator << (std::ostream & out, matrix_view<ValueType,OptionsFlags,TraversalOrder> const &);

  template <typename ValueType, ull_t OptionsFlags=0> 
  std::ostream & operator << (std::ostream & out, vector_view<ValueType,OptionsFlags> const &);

Prints the view (or the container since the view is trivially build from the container) 
to the stream out.




.. highlight:: c
 
.. _arr_stream:

operator << 
==================================

**Synopsis**::

  template <typename ValueType, int Rank, typename TraversalOrder=void> 
  std::ostream & operator << (std::ostream & out, array_view<ValueType,Rank,TraversalOrder> const &);
   
  template <typename ValueType, typename TraversalOrder=void> 
  std::ostream & operator << (std::ostream & out, matrix_view<ValueType,TraversalOrder> const &);

  template <typename ValueType> 
  std::ostream & operator << (std::ostream & out, vector_view<ValueType> const &);

Prints the view (or the container since the view is trivially build from the container) 
to the stream out.




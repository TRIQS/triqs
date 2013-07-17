How to write quickly and efficently an iterator
======================================================

* Do not use too much iterators, usually for loops are more efficient.

* Iterators are mainly useful in combination with STL.

* The best way to have an STL compliant iterator is to use the boost::iterator_facade documented
  `here <http://www.boost.org/doc/libs/1_49_0/libs/iterator/doc/iterator_facade.html#tutorial-example>`_

Example (stupid since std::vector already has an iterator) ::

 #include <boost/iterator/iterator_facade.hpp>

  template <typename T>
  class my_iterator :  public boost::iterator_facade< my_iterator<T> , const std::ptrdiff_t , boost::forward_traversal_tag > {
    public:
     typedef CuboidMap indexmap_type;
     typedef typename indexmap_type::domain_type domain_type;
     typedef IterationOrder iteration_order;
     typedef typename domain_type::index_value_type indices_type;
     typedef const std::ptrdiff_t return_type;

     my_iterator (): Parent(NULL), pos(0),atend(true) {}
     my_iterator (const indexmap_type & P, bool atEnd=false): Parent(&P), pos(Parent->start_shift()),atend(atEnd) {}

     indices_type const & indices() const { return indices_tuple; }
     operator bool() const { return !atend;}

    private:
     friend class boost::iterator_core_access;
     void increment(); // below
     bool equal(my_iterator const & other) const {return ((other.Parent==Parent)&&(other.atend==atend)&&(other.pos==pos));}
     return_type & dereference() const { assert (!atend); return pos; }

     const indexmap_type * Parent;
     indices_type indices_tuple; 
     std::ptrdiff_t pos;
     bool atend;
   };



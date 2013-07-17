.. highlight:: c

Lazy Objects
====================

PreCompute in Lazy Object
-----------------------------

* why a lazy object
* example

* idea : keep a view or a copy and then implement the concept.
* add possibly some optimisation

* When precomputation are needed.
  --> LazyPreCompute pattern with the shared_ptr
 
   Ex ::

    class MyLazyObject {

     // ..
     private: 
     
     struct internal_data { 
      // contains the data for the precomputation
      // e.g.  std::vector<int> data;
      internal_data(MyLazyObject const & P) { 
       // do the computation and fill the data
       // data = ...;
      }
     };
     friend struct internal_data;   // internal_data can read the parent object P
     mutable boost::shared_ptr<internal_data> _id;   // holds the data
     void activate() const { if (!_id) _id= boost::make_shared<internal_data>(*this);}

    } 

Comments :

* The data is hold in a specialized struct `internal_data`, which contains the precomputation.
* The data is hold by a shared_ptr, _id : 
   
  * The copy of MyLazyObject is then cheap and safe with shared_ptr.
  * If the computation is not necessary, _id is empty, there is no memory usage.
  * activate() activates the calculation of the internal data.
  * _id is mutable : the precomputation is private, so even a const object should be able to activate safely.

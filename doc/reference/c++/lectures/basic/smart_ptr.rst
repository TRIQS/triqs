.. highlight:: c

Pointers : from the evil to the smart...
######################################################

Raw pointers
===============

Raw pointers are ordinary C/C++ pointers.

Pointers allow to manipulate objects by reference (or indirectly).

RefTo : stack/heap. Pointers in C++...

They are used e.g. to allocate a new object on the heap ::
  
   {
    A * p = new A( constructor_arguments);  // allocated and constructed on the heap.
    A a( constructor_arguments);            // constructed on the stack
   
    delete p;                               // release memory
   }

Raw pointers are the source of many bugs in C++. Two main dangers are  :

* memory leak : when one forget to release the memory allocated to the pointer::

   {
    A * p = new A( constructor_arguments);  // allocated and constructed on the heap.
    //
    // forgot to delete. but p is gone....
   }

* a more subtle memory leak ::

   {
    A * p = new A( constructor_arguments);  // allocated and constructed on the heap.
    // working with p...
    delete p;
   }

  What happens if there is an exception during the work ??
  The pointer is not deallocated (but all objects on stack would be destroyed).

* dangling pointers : reusing a pointer after the object has been deleted ::

   { 
    A * p = new A( constructor_arguments);  // allocated and constructed on the heap.
    //...
    A *q =p; a copy of the pointer is made somewhere
    //...
    delete p;
    // ...
    q->method(..) ; // DEAD : segfault.
   }

To conclude :

* pointers are a low level object

* pointers induce many dangers...

The solution 
====================

Smart pointer are a way to manipulate objects by reference with strong guarantee
that such common problems will **never** happen.

The bottom line can be summarized as :

 *Smart pointers are pointers with strong guarantee to avoid dangling references and memory leaks.*


In order to avoid pointers problems, the style recommendation are : 

#. Try to avoid pointers ! Use object by value, and objects like std::vector
   which hide the dynamic allocation of memory from the user in a safe manner.

#. If you need a pointer to an object A which is pointed to by exactly one pointer
   (a local object), then use std::unique_ptr (or boost:scoped_ptr if you insist on non C++11).

#. If you need an object A shared by multiple objects, i.e. pointed to by several pointers
   use a std::shared_ptr.

#. Only when other options are not good, and in practice only at low level libraries,
   use raw pointers to allocate objects, with new and delete.

std::unique_ptr
---------------------

Example ::

   {
    std::unique_ptr<A> p( new A( constructor_arguments) );  // allocated and constructed on the heap and stored.
    //
    // working with p. p has the semantics of a pointer : 
    // p-> method(), p->data, etc ...
    // NO DELETE : when the std::unique_ptr is getting out of scope and is destroyed the 
    // underlying raw pointer are freed.
   }

Note that : 

* a unique_ptr **can not be copied**. If you put it as a member of a class, this class can therefore not be copied.

* a unique_ptr **can be moved** (see lecture on move semantics).

* This writing is exception-safe. If an exception occurs in the treatment, then C++ guarantees
  the all the local objects are destroyed, hence p and the underlying raw pointer is deleted.

* unique_ptr has little if no overhead in critical code.


std::shared_ptr
---------------------

(or boost::shared_ptr < C++11).

In the case where we want to **share** an object, i.e. that multiple pointers points to an object,
we have to use std::shared_ptr.

A shared_ptr is no more than :

* a pointer to the object

* a reference counter, counting how many shared_ptr point to the objects.


When the shared_ptr are copied, assigned, the reference counter is updated accordingly.

* When a shared_ptr is created, copied, ref_counter is increased by 1.
* When a shared_ptr is deleted, ref_counter is decreased by 1.

When the last shared_ptr to an object is destroyed, the reference counter gets
to 0 and the pointed objects is deleted.

Example ::

  class A;
  
  boost::shared_ptr<A> p (construction_parameters);

  // work with p like a pointer
  // p has pointer semantic. *p, p->method as for raw pointer

  // NO delete


Example ::

  class A;
  
  boost::shared_ptr<A> p1 (construction_parameters);
  boost::shared_ptr<A> p2 (construction_parameters);

  // p1 ---->  ( a1, ref_counter = 1 )
  // p2 ---->  ( a2, ref_counter = 1 )
   
  { // group start 
    boost::shared_ptr<A> p3 = p2;
    // p1 ---->  ( a1, ref_counter = 1 )
    // p2 ---->  ( a2, ref_counter = 2 )
    // p3 ---->  ( a2, ref_counter = 2 )
  
   } // group ends ... p3 is deleted
    
   // p1 ---->  ( a1, ref_counter = 1 )
   // p2 ---->  ( a2, ref_counter = 1 )

   p1 = p2;
   // p1 ---->  ( a1, ref_counter = 2 )
   // p2 ---->  ( a1, ref_counter = 2 )
   // a2 has been destroyed.



   






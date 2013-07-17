Type erasure : from static to dynamical polymorphism
############################################################

* What is the issue ?
* A basic example : std::function
* Example : 
* A real life example : QMC class.
* The classical C++ solution with virtual functions



Polymorphism
================

Static polymorphism (template, generic programming)
---------------------------------------------------------

Dynamical polymorphism.

The crossing : why ? How ?
Concepts. When is the decision taken : at compile time or a run time ?

A simple example : std::function
Can handle any function or object which ahs the concept double(int,int) e.g.
It **erases** the type of the object.

The pb : find an envelop for an object with : 
 * the concept 
 * one type, that can handle any type.
---> Add Abrahams definitions ( contruct + =).

An example : 

Pb is twofold : 
* keep the undelying alive
* call its methods wihout remembering its type.

--> show a solution 
Use std::function + std::bind variant without lambdas.
Use shared_ptr<void>
* shared_ptr<void> the universal handle

--> Hum : pb : restore the value semantics !

* The traditionnal C++ way using 3 classes. ugly...

The envelop as a concept checker ???
--> It does not compile it T does not model the concept.

Python : a general type eraser : PyObject * or boost::python::object.

2 frontiers : template, C++ polymorphism, Python : in easy and in speed.




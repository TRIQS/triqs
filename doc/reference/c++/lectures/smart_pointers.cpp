#include <iostream>
#include <boost/shared_ptr.hpp>


class myObject{

	int i;

   public:

	myObject(){}
	myObject(int i_):i(i_){}
	int getValue() const {return i;}
};



struct A{

	
};

struct B{
	int data;
	boost::shared_ptr<A> p;
};

struct C{
	boost::shared_ptr<A> p;
};


int main(){

	/* WHY POINTERS? */


	

	myObject A(0); // object is allocated on the stack

	/* stack VS heap */
	// small variables on the stack
	// dynamical memory on the heap == "new"

	//when running, the OS has allocated the stack size...
	//when allocatinh by new ==> HEAP, one asks the kernel for space in the system ... takes time!
	//RULE: in critical loops, do not allocate using new !

	//stack: only for objects whose size is known at compile time

	//array lib : minivector on stack, to avoid allocation problems (ulimit...


	std::cout << A.getValue()<<std::endl;

	/* USUAL POINTERS */
	myObject * p1;
	p1 = new myObject(1); //allocate on the heap

	myObject * p2(new myObject(2)); // shorter way


	// two equivalent ways of accessing public members
	std::cout << p1->getValue() << std::endl;
	std::cout << (*p1).getValue() << std::endl;



	/* THE ISSUE */
	delete(p1); // deletes the pointer but not the underlying data
	

	
	/* SHARED POINTER */
	boost::shared_ptr<myObject> p1_sh(new myObject(1));

	std::cout << p1_sh->getValue()<<std::endl;

	boost::shared_ptr<myObject> p2_sh = p1_sh;

	std::cout << p2_sh->getValue()<<std::endl;

	//never delete it!!
	/* difference betw pointer and reference : dynamical polymorphism */


	

	return 0;
}

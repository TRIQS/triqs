Basics
------

Here, an exemple of creation of a class det_manip, of use of insert and remove. 

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>

    struct fun {

    typedef double result_type;
    typedef double argument_type;

    //gives the coefficients of the matrix
    double operator()(double x, double y) const {
      return(x-y);
    }
    };

    int main() {
      
      //creation of the class det_manip
      fun f;
      int init_size=100;
      triqs::det_manip::det_manip<fun> D(f,init_size);
      //the initial matrix is empty:
      std::cout<<std::endl<<"After construction: D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      
      double detratio;
      //insertion of a line and a column at position (1,1) in the matrix, with x[i]=x, y[j]=y. 
      double x=2, y=9;
      int i=0, j=0;
      detratio = D.try_insert(i,j,x,y);
      D.complete_operation();
      std::cout<<"We add a line and a column for i="<<i<<", j="<<j<<", x="<<x<<", y="<<y<<" (f(x,y)="<<f(x,y)<<")."<<std::endl;
      std::cout<<"D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      
      x=3; y=4; i=0; j=0;
      detratio = D.try_insert(i,j,x,y);
      D.complete_operation();
      std::cout<<"We add a line and a column for i="<<i<<", j="<<j<<", x="<<x<<", y="<<y<<" (f(x,y)="<<f(x,y)<<")."<<std::endl;
      std::cout<<"D.matrix()="<<D.matrix()<<std::endl;
      std::cout<<"The determinant is "<<D.determinant()<<std::endl;
      std::cout<<"The inverse matrix is"<<D.inverse_matrix()<< std::endl<<std::endl;
      
      x=-7; y=1; i=1; j=2;
      detratio = D.try_insert(i,j,x,y);
      D.complete_operation();
      std::cout<<"We add a line and a column for i="<<i<<", j="<<j<<", x="<<x<<", y="<<y<<" (f(x,y)="<<f(x,y)<<")."<<std::endl;
      std::cout<<"D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      std::cout<<"The size of the matrix is now "<<D.size()<<std::endl;
      std::cout<<"The value of the parameters for coefficient (i,j)=(0,2) is (x,y)=("<<D.get_x(0)<<","<<D.get_y(2)<<") (f("<<D.get_x(0)<<","<<D.get_y(2)<<")="<<f(D.get_x(0),D.get_y(2))<<")."<<std::endl<<std::endl;

      i=0; j=1;
      detratio = D.try_remove(i,j);
      D.complete_operation();
      std::cout<<"We remove a line and a column for i="<<i<<", j="<<j<<"."<<std::endl;
      std::cout<<"D.matrix()="<<D.matrix()<<std::endl;
      std::cout<<"The determinant is "<<D.determinant()<<std::endl;
      std::cout<<"The inverse matrix is"<<D.inverse_matrix()<< std::endl<<std::endl;
    }



Learn more in the full reference, see :ref:`det_manip`



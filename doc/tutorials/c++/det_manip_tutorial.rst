Det_manip cookbook
===================

.. highlight:: c

.. toctree::
   :maxdepth: 1

TRIQS comes with a class called det_manip to easily perform operations on a special type of matrices 
(see  :doc:`here  <../../reference/c++/det_manip/contents>`). 
This library, among others, allows to easily add or remove lines or columns to the matrix, to calculate the determinant and the inverse. 
Here are a couple of simple examples showing the basic use of this class.



Creation of an empty det_manip class
-------------------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>

    struct fun {

      typedef double result_type;
      typedef double argument_type;

      //gives the coefficients of the matrix (function F of the documentation)
      double operator()(double x, double y) const {
        return(x-y);
      }

    };

    int main() {
    
      fun f;
      int init_size = 100; // maximum size of the matrix before a resize
      
      //creation of a class det_manip
      triqs::det_manip::det_manip<fun> D(f, init_size);
      
      //the initial matrix is empty:
      std::cout<<std::endl<< "After construction: D.matrix()=" << D.matrix()<<std::endl<<std::endl;
      
    }

Creation of a non empty det_manip class
----------------------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>
    
    struct fun {
      
      typedef double result_type;
      typedef double argument_type;
      
      //gives the coefficients of the matrix (function F of the documentation)
      double operator()(double x, double y) const {
        return(x-y);
      }
      
    };
    
    int main() {
      
      fun f;
      std::vector<double> initial_x{1,2}, initial_y{3,4};
      
      //creation of a class det_manip with a 2 by 2 matrix
      triqs::det_manip::det_manip<fun> D(f, initial_x, initial_y);
      
      //the initial matrix:
      std::cout<<std::endl<< "After construction: D.matrix()=" << D.matrix()<<std::endl<<std::endl;
      
    }


Get informations about a det_manip class
-----------------------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>
    
    struct fun {
     typedef double result_type;
     typedef double argument_type;
     double operator()(double x, double y) const { return(x-y); }
    };
    
   int main() {
    fun f;
    int i=0, j=1;
    std::vector<double> initial_x{1,2}, initial_y{3,4};
    triqs::det_manip::det_manip<fun> D(f, initial_x, initial_y);
    std::cout<<std::endl<<"D.matrix()=" << D.matrix() <<std::endl<<std::endl;
    std::cout<<"The size of the matrix is "<< D.size() <<std::endl<<std::endl;
    std::cout<<"The determinant is "<< D.determinant() <<std::endl<<std::endl;
    std::cout<<"The inverse matrix is"<< D.inverse_matrix() << std::endl<<std::endl;
    std::cout<<"The value of the parameters for coefficient (i,j)=("<<i<<","<<j<<") is (x,y)=("
    <<D.get_x(i)<<","<<D.get_y(j)<<")"<<std::endl<<std::endl;
   }

Add a line and a column
-------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>
    
    struct fun {
      typedef double result_type;
      typedef double argument_type;
      double operator()(double x, double y) const { return(exp(x)-y*y); }
    };
    
    int main() {
      triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1,2,2.5}, std::vector<double>{3,4,9});
      std::cout<<std::endl<<"After construction, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      double x0 = 2.1, y0 = 7;
      int i = 2, j = 0; // number of the added line and column
      std::cout<<"We want to add a line and a column for i="<<i<<", j="<<j
      <<", x="<<x0<<", y="<<y0<<"."<<std::endl;
      // (try of) insertion of a line and a column at position (3,1) in the matrix
      // with x[i]=x0, y[j]=y0. 
      double detratio = D.try_insert(i, j, x0, y0); // the ratio between new and old determinants
      // while the operation is not complete, the matrix stays unchanged
      std::cout<<"After try_insert, D.matrix()="<<D.matrix()<<std::endl;
      // here we validate the insertion: the (inverse) matrix and determinant are updated
      D.complete_operation();
      std::cout<<"After complete_operation, D.matrix()="<<D.matrix()<<std::endl<<std::endl;

    }

Remove a line and a column
---------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>
    
    struct fun {
      typedef double result_type;
      typedef double argument_type;
      double operator()(double x, double y) const { return(exp(x)-y*y); }
    };
    
    int main() {
      triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1,2,2.5}, std::vector<double>{3,4,9});
      std::cout<<std::endl<<"After construction, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      int i = 1, j = 0; // number of the removed line and column
      std::cout<<"We want to remove a line and a column for i="<<i<<", j="<<j<<"."<<std::endl;
      // (try of) removal of a line and a column at position (1,0) in the matrix.
      double detratio = D.try_remove(i, j); // the ratio between new and old determinants
      // while the operation is not complete, the matrix stays unchanged
      std::cout<<"After try_remove, D.matrix()="<<D.matrix()<<std::endl;
      // here we validate the removal: the (inverse) matrix and determinant are updated
      D.complete_operation();
      std::cout<<"After complete_operation, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
    }



Add two lines and two columns
------------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>
    
    struct fun {
      typedef double result_type;
      typedef double argument_type;
      double operator()(double x, double y) const { return(exp(x)-y*y); }
    };
    
    int main() {
      triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1,2,2.5}, std::vector<double>{3,4,9});
      std::cout<<std::endl<<"After construction, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      double x0 = 2.1, y0 = 7, x1 = 3.5, y1 = 5;
      int i0 = 2, i1 = 1, j0 = 0, j1 = 3;      // number of the added lines and columns
      std::cout<<"We want to add a line and a column for i0="<<i0<<", j0="<<j0<<", i1="<<i1<<", j1="<<j1
      <<", x0="<<x0<<", y0="<<y0<<", x1="<<x1<<", y1="<<y1<<")."<<std::endl;
      // (try of) insertion of 2 lines and 2 columns in the matrix
      double detratio = D.try_insert2 (i0, i1, j0, j1, x0, x1, y0, y1); // the ratio between new and old determinants
      // while the operation is not complete, the matrix stays unchanged
      std::cout<<"After try_insert2, D.matrix()="<<D.matrix()<<std::endl;
      // here we validate the insertion: the (inverse) matrix and determinant are updated
      D.complete_operation();
      std::cout<<"After complete_operation, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
    }

Remove two lines and two columns
--------------------------------

.. compileblock:: 

    #include <triqs/det_manip/det_manip.hpp>
    
    struct fun {
      typedef double result_type;
      typedef double argument_type;
      double operator()(double x, double y) const { return(exp(x)-y*y); }
    };
    
    int main() {
      triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1,2,2.5}, std::vector<double>{3,4,9});
      std::cout<<std::endl<<"After construction, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
      int i0 = 2, i1 = 1, j0 = 0, j1 = 1;      // number of the removed lines and columns
      std::cout<<"We want to remove 2 lines and 2 columns for i0="
      <<i0<<", j0="<<j0<<", i1="<<i1<<", j1="<<j1<<"."<<std::endl;
      // (try of) removal of a line and a column at position (1,0) in the matrix.
      double detratio = D.try_remove2(i0, i1, j0, j1); // the ratio between new and old determinants
      // while the operation is not complete, the matrix stays unchanged
      std::cout<<"After try_remove2, D.matrix()="<<D.matrix()<<std::endl;
      // here we validate the removal: the (inverse) matrix and determinant are updated
      D.complete_operation();
      std::cout<<"After complete_operation, D.matrix()="<<D.matrix()<<std::endl<<std::endl;
    }


Learn more in the full reference, see :ref:`det_manip`



.. highlight:: c

The det_manip class
====================

Parameter & construction
------------------------

The template parameter is the **FunctionType**, the type of F,
which is completely general but has to model the concept

  * return_type : type returned by the function 
  * argument_type : type of the argument of the function (type of x and y).

Operations
----------

* The possible operations on the matrix M are : 

+------------+--------------------------------+
| Operation  | Effect                         |
+============+================================+
| insert     | adding a line and a column     |
+------------+--------------------------------+
| remove     | removing a line and a column   |
+------------+--------------------------------+
| insert2    | adding 2 lines and 2 columns   |
+------------+--------------------------------+
| remove2    | removing 2 lines and 2 columns |
+------------+--------------------------------+
| change_col | changing one *y*               |
+------------+--------------------------------+
| change_raw | changing one *x*               |
+------------+--------------------------------+


* Each operation *OP* is called in two steps: 

  * 
    .. code-block:: c

       value_type try_OP(arguments ...) 

    Returns the ratio 

    .. math:: \frac{det M'}{det M}

    where M' would be the matrix after the operation is completed.

    try_OP **does NOT** modify the matrix :math:`M`.

  * 
    .. code-block:: c
   
       void complete_operation() 

    Complete the last operation OP (the last called try_OP), by updating the list of x and y 
    and the inverse of the matrix to :math:`(M')^{-1}`.

* This structure is designed to write  Monte Carlo algorithms : 
  
  * the try part of the move calls some try_OP
  * if and only if the move is accepted, is the complete_operation called.

Under the hood ...
-------------------------

* All matrix algebra is made with BLAS calls.

* The storage is done in a compact way: when a column or row is added, 
  no data are shifted, it is added at the end of the matrix.
  However, the permutation of row and columns are handled by this class
  so that this is transparent for the user.


Doxygen documentation
-------------------------

The :doxy:`full C++ documentation<triqs::det_manip::det_manip>` is available here.

Example  
-------------

.. code-block:: c

 struct fun {

  typedef double result_type;
  typedef double argument_type;

  double operator()(double x, double y) const { 
    const double pi = acos(-1); 
    const double beta = 10.0; 
    const double epsi = 0.1; 
    double tau = x-y;
    bool s = (tau>0);
    tau = (s ? tau : beta + tau);
    double r = epsi + tau/beta * (1-2*epsi);
    return - 2*(pi/beta)/ std::sin ( pi*r);
  }
 };

 int main() { 

   fun f;
   triqs::det_manip::det_manip<fun> D;

   /// ....
   double x=2, y=9, detratio; 
   detratio = D.try_insert(1,3, x,y); 
   D.complete_operation();
   
   ///...
   detratio = D.try_remove(2,1);
   D.complete_operation();
  }







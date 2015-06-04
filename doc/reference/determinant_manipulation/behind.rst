.. highlight:: c

How does it work ?
###################

We give here the mathematical formulas used in the det_manip class. 

Cofactors
==========

For any :math:`n\times n` matrix :math:`A`:

.. math:: A\,{\rm Cof}(A^T) = {\rm Det}A\, I_n.

where :math:`\rm{Cof}` means the matrix of the cofactors. 

.. math::    {\rm Cof}(A)_{i,j}
             =(-1)^{i+j}{\rm Det}\begin{pmatrix}
             a_{1,1}   & \dots & a_{1,j-1}   & a_{1,j+1}  & \dots & a_{1,n}   \\
             \vdots    &       & \vdots      & \vdots     &       & \vdots    \\
             a_{i-1,1} & \dots & a_{i-1,j-1} & a_{i-1,j+1}& \dots & a_{i-1,n} \\
             a_{i+1,1} & \dots & a_{i+1,j-1} & a_{i+1,j+1}& \dots & a_{i+1,n} \\
             \vdots    &       & \vdots      & \vdots     &       & \vdots    \\
             a_{n,1}   & \dots & a_{n,j-1}   & a_{n,j+1}  & \dots & a_{n,n}   \end{pmatrix}.

    
Addition of a line and a column
==================================

:math:`A` is an inversible :math:`n\times n` matrix. :math:`A'` is a :math:`(n+1)\times (n+1)` matrix obtained by adding a line and a column to :math:`A`:

.. math:: A'=\begin{pmatrix} 
          A & B \\ 
          C & D \end{pmatrix}.

Using the previous formula with the cofactors, we get

.. math::  \frac{{\rm Det}A'}{{\rm Det}A}=D-C A^{-1} B. 

Using the following variables:

.. math:: \xi=D-C  A^{-1} B, \qquad B'=A^{-1}B, \qquad C'=CA^{-1}, 

We get the inverse of the new matrix as:

.. math:: (A')^{-1}=
          \begin{pmatrix}
            A^{-1}+\xi^{-1} B'C' & -\xi^{-1} B'\\
            -\xi^{-1} C'         &  \xi^{-1}
          \end{pmatrix}


Removal of a line and a column
=================================

Using:

.. math:: (A)^{-1}=
          \begin{pmatrix}
            F & G\\
            H & d
          \end{pmatrix}

We get the inverse of the new matrix as:
          
.. math:: (A')^{-1}=
          \begin{pmatrix}
            F + GH/d
          \end{pmatrix}

and :math:`d` is the new determinant. 

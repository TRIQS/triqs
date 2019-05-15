.. highlight:: c

How does it work ?
###################

We give here the mathematical formulas used in the det_manip class. 
As this class mainly contains the determinant and the inverse of a matrix :math:`A`, we need to know how to update these two members as operations are performed (addition/removal/replacement of one/two lines). 

Cofactors
==========

For any :math:`n\times n` matrix :math:`A`, its inverse (if it is defined) is related to the matrix of its cofactors :math:`\rm{Cof}` through:

.. math:: A\,{\rm Cof}(A^T) = {\rm Det}A\, I_n.

The matrix of the cofactors is defined as: 

.. math::    {\rm Cof}(A)_{i,j}
             =(-1)^{i+j}{\rm Det}\begin{pmatrix}
             a_{1,1}   & \dots & a_{1,j-1}   & a_{1,j+1}  & \dots & a_{1,n}   \\
             \vdots    &       & \vdots      & \vdots     &       & \vdots    \\
             a_{i-1,1} & \dots & a_{i-1,j-1} & a_{i-1,j+1}& \dots & a_{i-1,n} \\
             a_{i+1,1} & \dots & a_{i+1,j-1} & a_{i+1,j+1}& \dots & a_{i+1,n} \\
             \vdots    &       & \vdots      & \vdots     &       & \vdots    \\
             a_{n,1}   & \dots & a_{n,j-1}   & a_{n,j+1}  & \dots & a_{n,n}   \end{pmatrix}.



The Sherman-Morrison formula
=============================
    
:math:`A` is an invertible :math:`n\times n` matrix, and :math:`u`, :math:`v` are :math:`n` vectors.
Suppose furthermore that :math:`1+v^t A^{-1} u != 0`.
Then the Sherman-Morrison formula states that

.. math:: (A + u v^t)^{-1} = A^{-1} - \frac{ A^{-1} u v^t A^{-1} }{ 1 + v^t A^{-1} u}

Similar formula for the determinant:

.. math:: {\rm Det}(A + u v^t) = Det(A)(1 + v^t A^{-1} u).



Addition of a line and a column, or more
=========================================

:math:`A` is an invertible :math:`n\times n` matrix. :math:`A'` is a :math:`(n+1)\times (n+1)` matrix obtained by adding a line and a column to :math:`A`:

.. math:: A'=\begin{pmatrix} 
          A & B \\ 
          C & D \end{pmatrix}.

:math:`B` is a column and :math:`C` a line both of size :math:`n`.
:math:`D` is a scalar. 

Using the following variables:

.. math:: \xi=D-C A^{-1} B, \qquad B'=A^{-1}B, \qquad C'=CA^{-1}, 

and the previous formula with the cofactors, we get the determinant and the inverse of :math:`A'` as:

.. math::  \frac{{\rm Det}A'}{{\rm Det}A}={\rm Det}\xi. 

.. math:: (A')^{-1}=
          \begin{pmatrix}
            A^{-1}+ B'\xi^{-1}C' & -B'\xi^{-1} \\
            -\xi^{-1}C'          &  \xi^{-1}
          \end{pmatrix}

For the addition of two or more lines and columns, the formulas remain the same, but :math:`B`, :math:`C`, :math:`D`, :math:`\xi`, :math:`B'` and :math:`C'` are now matrices of different sizes.



Removal of a line and a column, or more
========================================

We remove the last line and column of the matrix :math:`A` of size :math:`(n+1)\times (n+1)`.
Writing:

.. math:: (A)^{-1}=
          \begin{pmatrix}
            F & G\\
            H & I
          \end{pmatrix}

where :math:`G` is a column and :math:`H` a line both of size :math:`n`, 
:math:`I` is a scalar, 
we get the determinant and the inverse of the new matrix :math:`A'` as:
          
.. math:: \frac{{\rm Det}A'}{{\rm Det}A}={\rm Det}I,

.. math:: (A')^{-1}=
          \begin{pmatrix}
            F - GI^{-1}H
          \end{pmatrix}.

For the removal of two or more lines and columns, the formulas remain the same, but :math:`F`, :math:`G`, :math:`H`, and :math:`I` are now matrices of different sizes. 



Change of a column
===================

:math:`A'` is the new matrix of size :math:`n\times n`, where the last column has been changed:
 
Writing:

.. math:: A'=
          \begin{pmatrix} 
            A_0 & B+\Delta B 
          \end{pmatrix}.

We know the inverse of the old matrix :math:`A` (of the same size as :math:`A'`): 

.. math:: A^{-1}=
          \begin{pmatrix}
            F\\
            G
          \end{pmatrix}

Using the following variables:

.. math:: M = A^{-1}\Delta B, 

We have

.. math:: (A')^{-1} = A^{-1} - \frac{ M G }{ 1 + M_n }

.. math:: \frac{{\rm Det}A'}{{\rm Det}A}= 1 + M_n

To ameliorate the precision of the algorithm, the last line of :math:`A^{-1}` should only be divided by :math:`1 + M_n` and not updated by the general formula. 


Change of a line
===================

It is a straightformward adaptation of the previous section:
 
Writing:

.. math:: A'=
          \begin{pmatrix} 
            A_0 \\ C+\Delta C 
          \end{pmatrix}.

We know the inverse of the old matrix :math:`A` (of the same size as :math:`A'`): 

.. math:: A^{-1}=
          \begin{pmatrix}
            F & G
          \end{pmatrix}

Using the following variables:

.. math:: M =\Delta C A^{-1}, 

We have

.. math:: (A')^{-1} = A^{-1} - \frac{ G M }{ 1 + M_n }

.. math:: \frac{{\rm Det}A'}{{\rm Det}A}= 1 + M_n

To ameliorate the precision of the algorithm, the last column of :math:`A^{-1}` should only be divided by :math:`1 + M_n` and not updated by the general formula. 



Change of a line and a column
===============================
 
:math:`A'` is the new matrix of size :math:`n\times n` where the last line and column has been changed:
 
Writing:

.. math:: A'=\begin{pmatrix} 
          A_0 & B \\ 
          C & D \end{pmatrix}.

We know the inverse of the old matrix :math:`A` (of the same size as :math:`A'`): 

.. math:: (A)^{-1}=
          \begin{pmatrix}
            F & G\\
            H & I
          \end{pmatrix}

Using the following variables:

.. math:: h = HB, \qquad g=CG, 

.. math:: C'=CF, \qquad B'=FB, 

.. math:: f=C'B-D=CB'-D, \qquad \xi=gh-If, 

.. math:: G'=\xi^{-1}(hG-IB'), \qquad H'=\xi^{-1}(gH-IC'),

.. math:: F' = F-G'C'+\xi^{-1}(fG-gB')H
             = F-B'H'+\xi^{-1}G(fH-hC')

We get

.. math:: (A')^{-1}=
          \begin{pmatrix}
            F' & G'\\
            H' & \xi^{-1}I
          \end{pmatrix}

.. math:: \frac{{\rm Det}A'}{{\rm Det}A}=\xi

Note that this formulas remain valid if :math:`A_0` is not invertible.


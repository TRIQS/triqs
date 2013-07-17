/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#ifndef TRIQS_ARRAY_EIGENELEMENTS_H
#define TRIQS_ARRAY_EIGENELEMENTS_H
#include <type_traits>
#include "../matrix.hpp"
#include "../vector.hpp"
#include <triqs/utility/exceptions.hpp>

namespace triqs { namespace arrays { namespace linalg { 

 /**
  * A worker to call lapack routine with the matrices.
  *
  * Handles both real and complex case.
  *
  * Usage : 
  *  - construct from a VIEW of a matrix, that MUST be contiguous.
  *  - call invoke()
  *  - read the eigenvalues/vectors in values and vectors resp.
  *  NB : the content of the matrix is destroyed by the computation (it is .vectors() in fact, if Compute_Eigenvectors is true).
  *  For a one shot usage, prefer eigenelements, eigenvalues functions.
  */
 template<typename MatrixViewType, bool Compute_Eigenvectors > struct eigenelements_worker;
 template<typename T, ull_t Opt, bool Compute_Eigenvectors > struct eigenelements_worker_base;

 template<typename T, ull_t Opt >
  struct eigenelements_worker_base <T,Opt,false> { 
   private:
    void operator = ( eigenelements_worker_base const & x);
   protected:
    matrix_view <T,Opt> mat;
    triqs::arrays::vector<double> ev; 
    triqs::arrays::vector<T> work;
    int dim,info,lwork;
    char uplo,compz;
    bool has_run;

    eigenelements_worker_base ( matrix_view <T,Opt> the_matrix) : mat(the_matrix) { 
     if (mat.is_empty())   TRIQS_RUNTIME_ERROR<<"eigenelements_worker : the matrix is empty : matrix =  "<<mat<<"  ";
     if (!mat.is_square())   TRIQS_RUNTIME_ERROR<<"eigenelements_worker : the matrix "<<mat<<" is not square ";
     if (!mat.indexmap().is_contiguous())   TRIQS_RUNTIME_ERROR<<"eigenelements_worker : the matrix "<<mat<<" is not contiguous in memory";
     dim = mat.dim0();
     ev.resize(dim);
     lwork = 64*dim;
     work.resize(lwork);
     uplo='U';compz='N' ;
     has_run = false;
    }

   public :
    array<double,1> values() const { 
     if (!has_run)  TRIQS_RUNTIME_ERROR<<"eigenelements_worker has not been invoked !";
     return ev;
    }
  };

 //--------------------------------

 template<typename T, ull_t Opt>
  struct eigenelements_worker_base <T,Opt,true> : eigenelements_worker_base <T,Opt,false>  {
   protected:
    eigenelements_worker_base ( matrix_view <T,Opt> the_matrix) :  eigenelements_worker_base <T,Opt,false>  (the_matrix) {this->compz='V'; }
   public:
    matrix<T,Opt> vectors() const { 
     if (!this->has_run)  TRIQS_RUNTIME_ERROR<<"eigenelements_worker has not been invoked !";
     return this->mat;
    }
  };

 //--------------------------------
 extern "C" { 
 void TRIQS_FORTRAN_MANGLING(dsyev)(char*,char*,        // JOBZ and UPLO
   int &,              // Matrix Size
   double[],            // matrix
   int&,               // LDA of the matrix
   double[],           // Eigenvalues array
   double[],            // WORK
   int&,               // LWORK
   int &               // INFO
   );


 void TRIQS_FORTRAN_MANGLING(zheev)(char*,char*,        // JOBZ and UPLO
   int &,              // Matrix Size
   std::complex<double> [],            // matrix
   int&,               // LDA of the matrix
   double[],           // Eigenvalues array
   std::complex<double>[],            // WORK
   int &,               // LWORK
   double[],  // WORK2
   int &               // INFO
   );
 }

 //--------------------------------

 template<ull_t Opt, bool Compute_Eigenvectors >
  struct eigenelements_worker< matrix_view<double,Opt> ,Compute_Eigenvectors > :eigenelements_worker_base<double,Opt,Compute_Eigenvectors> { 
   eigenelements_worker ( matrix_view <double,Opt> the_matrix) : eigenelements_worker_base<double,Opt,Compute_Eigenvectors> (the_matrix) {}
   void invoke() {
    int info;
    //fortran_int_t info;
    TRIQS_FORTRAN_MANGLING(dsyev) (&this->compz,&this->uplo,this->dim,this->mat.data_start(),this->dim,this->ev.data_start(),this->work.data_start(),this->lwork,info);
    if (info)  TRIQS_RUNTIME_ERROR<<"eigenelements_worker :error code dsyev : "<<info<<" for matrix "<<this->mat;
    this->has_run = true;
   }
  };

 //--------------------------------

 template<ull_t Opt, bool Compute_Eigenvectors >
  struct eigenelements_worker< matrix_view<std::complex<double>, Opt>,Compute_Eigenvectors > :eigenelements_worker_base<std::complex<double>,Opt,Compute_Eigenvectors> { 
   triqs::arrays::vector <double> work2;   
   public :
   eigenelements_worker ( matrix_view <std::complex<double>,Opt> the_matrix) : eigenelements_worker_base<std::complex<double>,Opt,Compute_Eigenvectors> (the_matrix) {  work2.resize(this->lwork);}
   void invoke() {
    int info;
    TRIQS_FORTRAN_MANGLING(zheev) (&this->compz,&this->uplo,this->dim,this->mat.data_start(),
      this->dim,this->ev.data_start(),this->work.data_start(),this->lwork,this->work2.data_start(),info);
    if (info)  TRIQS_RUNTIME_ERROR<<"eigenelements_worker :error code zheev : "<<info<<" for matrix "<<this->mat;
    this->has_run = true;
   }
  };

 //--------------------------------

 /**
  * Simple diagonalization call, return all eigenelements.
  * Handles both real and complex case.
  * @param M : the matrix VIEW : it MUST be contiguous
  * @param take_copy : makes a copy of the matrix before calling lapack, so that the original is preserved.
  *   if false : no copy is made and the content of the matrix M is destroyed (it is equal to vectors()).
  *   if true : a copy is made, M is preserved, but of course it is slower...
  */
 template<typename MatrixViewType >   
  std::pair<array<double,1>, typename MatrixViewType::non_view_type> eigenelements( MatrixViewType M, bool take_copy =false) { 
   eigenelements_worker<MatrixViewType, true> W(take_copy ? make_clone(M)() : M()); W.invoke(); return std::make_pair(W.values(),W.vectors());
  }

 //--------------------------------

 /**
  * Simple diagonalization call, returning only the eigenvalues.
  * Handles both real and complex case.
  * @param M : the matrix VIEW : it MUST be contiguous
  * @param take_copy : makes a copy of the matrix before calling lapack, so that the original is preserved.
  *   if false : no copy is made and the content of the matrix M is destroyed.
  *   if true : a copy is made, M is preserved, but of course it is slower...
  */
 template<typename MatrixViewType >   
  triqs::arrays::vector_view <double> eigenvalues( MatrixViewType M, bool take_copy = false) { 
   eigenelements_worker<MatrixViewType,false> W(take_copy ? make_clone(M)() : M()); W.invoke(); return W.values(); 
  }

}}} // namespace triqs::arrays::linalg
#endif


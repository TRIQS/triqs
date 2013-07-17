
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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

#ifndef FULL_UPDATE_H_jh93n8
#define FULL_UPDATE_H_jh93n8

/* 
 * Usage : create such an object
 * transform it into boost::function<bool()> and pass it as AfterCycleDuty
*/
class full_update_mechanism { 
 int iter_fastup,nfastup;
 double FullUpdate_diff_ALERT,  FullUpdate_diff_increase, FullUpdate_diff_reduce;
 triqs::utility::report_stream & REPORT;

 public : 
 full_update_mechanism (triqs::utility::report_stream & rep_, 
   int NbreFastIteration_Init, int FullUpdate_diff_ALERT_, int FullUpdate_diff_increase_, int FullUpdate_diff_reduce_): REPORT(rep) {
  // fast update mechanism
  nfastup=NbreFastIteration_Init;
  iter_fastup=0;
  FullUpdate_diff_ALERT   = FullUpdate_diff_ALERT_;
  FullUpdate_diff_increase= FullUpdate_diff_increase_;
  FullUpdate_diff_reduce  = FullUpdate_diff_reduce_;
 }

 virtual ~full_update_mechanism() {}

 virtual double fullUpdate(){return 0;}

 bool operator()() {

  //Update the FullUpdate data if necessary
  iter_fastup++;
  // Here we take some decision, depending whether nfastup is too big, too small ,etc
  if ((nfastup>=0) && (iter_fastup>=nfastup)) {
   iter_fastup=0;
   // Now updating cleanly 
   double diff=this->fullUpdate();
   //REPORT <<  "Full update done" << endl;
   if (diff > FullUpdate_diff_ALERT) 
    REPORT <<"Full update : diff="<< diff <<"  WARNING : Fast Update deviation too big "<<endl;
   bool bo=false;
   if (diff < FullUpdate_diff_increase) {bo=true; nfastup=2*nfastup;}
   if (diff > FullUpdate_diff_reduce) {bo=true;nfastup=max(1,nfastup/2);}
   if (bo)  { 
    REPORT <<  "diff="<<diff<<endl;
    REPORT <<  "New number of fast updates : "<< nfastup<<endl<<endl;
   }
  }


 };

#endif



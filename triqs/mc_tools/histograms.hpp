
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

#ifndef STATISTIC_HISTOGRAM_H
#define STATISTIC_HISTOGRAM_H

#include <numeric>
#include <map>
#include <fstream>

namespace triqs {
namespace mc_tools {

 /**
   Histograms for a MonteCarlo run.
   For integer in [0,Size]
   */
 class histogram {
  protected:
   int del;
   std::vector<int> data;

  public : 
   /** */
   histogram(int Size):del(0),data(Size,0){}

   /** */
   histogram & operator<<(unsigned int i) 
   { if (i>data.size()) del++;else  data[i]++; return *this;}

   /** */
   inline void reset(){del=0; for (unsigned int u=0; u<data.size(); ++u) data[u]=0;}

   /** */
   void save(std::ostream & where){
    double norm = std::accumulate(data.begin(), data.end(), 0);
    for ( unsigned int i =0; i<data.size();i++)
     where << i<< "  "<< data[i]/double(norm)<<std::endl;
    if (del) std::cerr<<"Histogram : "<<del<<" points have been lost !"<<std::endl;
   }

   /** */
   void save(std::string s){ std::ofstream tmp(s.c_str(),std::ios::out); save (tmp);}

 }; 

 /**
   To accumulate histograms in a MonteCarlo run.
   Accumulates float in [a,b] with binning.
   */
 class histogram_binned : public histogram {
  public : 
   /** */
   histogram_binned(double a, double b, int Size=1000):histogram(Size), x0(a),LEN(b-a){
    if (!(LEN>0)) throw "histogram_binned construction : one must have a<b";
   }

   /** */
   inline histogram_binned & operator<<(double x) {
    int i = int(floor((x-x0)/LEN*data.size())); assert(i>=0);
    histogram::operator<< (static_cast<unsigned int>(i) ); 
    return *this;
   }

   /** */
   void save(std::ostream & where){
    double norm = std::accumulate(data.begin(), data.end(), 0);
    for ( unsigned int i =0; i<data.size();i++)
     where << i/double(data.size())*LEN<< "  "<< data[i]/norm<<std::endl;
    if (del) std::cerr<<"histogram_binned "<<" : "<<del<<" points have been lost !"<<std::endl;
   }

   /** */
   void save(std::string s){ std::ofstream tmp(s.c_str(),std::ios::out); save (tmp);}

  protected:
   double x0,LEN;
 }; 

 //
 typedef std::map<std::string,histogram_binned> HistogramBinnedMap;
 typedef std::map<std::string,histogram> HistogramMap;


}
}// end namespaces

#endif





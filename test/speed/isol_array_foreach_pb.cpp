#include <iostream>

void f (int * p,int i, int j, int val) { p[ i + 200*j] += val;}

int N1 = 200, N2 = 300;

int main() { 

 int * p = new int [ 200*300];
 int t[2];

#ifdef STATIC 
 const int u[2] = {1,0}; 
#else 
 int u[2]; for (int i=0; i<2; ++i) u[i] = 1-i;
#endif

  for (int k =0; k<500000; ++k)
    for (t[u[0]]=0; t[u[0]]<N2; ++t[u[0]]) 
   for (t[u[1]] =0; t[u[1]]<N1; ++t[u[1]])
     f(p,t[u[1]],t[u[0]] , 1 );

 std::cout<< " p[5] = " << p[5]<<std::endl ;
}

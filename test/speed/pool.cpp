#include <triqs/utility/first_include.hpp>
#include <vector>
#include <map>
#include <boost/pool/pool_alloc.hpp>
#include <ctime>
#include <iostream>

struct A {
 double x; int i;
};


typedef std::map< double, A, std::less<double>, boost::fast_pool_allocator< std::pair<const double, A >>> m_t;

template<typename M> void test(M & m) { 
 std::clock_t start = std::clock() ;
 for( int i = 0 ; i<100000 ; ++i ) {m.insert(std::make_pair( 10000*std::cos(i),A{0.5,i}));}
 std::cout << ( std::clock() - start ) / double(CLOCKS_PER_SEC) << " secs.\n" ;
}

int main()
{
 m_t m;
 test(m);

 std::map<double,A> m2; 
 test(m2);

 boost::singleton_pool<boost::pool_allocator_tag, sizeof(std::pair<const double, A>)>::release_memory();


}

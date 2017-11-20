#ifndef __cpp_if_constexpr
#error "C++17 support incomplete"
#endif

#ifndef __cpp_structured_bindings
#error "C++17 support incomplete"
#endif

int main() { 
 int i=2;
 auto f = [j =i]( auto x) { return x+j;};
}


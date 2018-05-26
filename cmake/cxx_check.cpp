
//#if not __has_feature(cxx_generic_lambdas)
//#error "C++14 support is not present"
//#endif

//#include <cstddef>
int main() {
  int i  = 2;
  auto f = [j = i](auto x) { return x + j; };
}

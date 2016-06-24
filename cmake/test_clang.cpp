#include <iostream>

#if not __has_feature(cxx_generic_lambdas)
#error "Clang is too old. C++14 support is not present"
#endif

int main() { 
 std::cout << __GNUC__ <<"."<<__GNUC_MINOR__ <<"."<<__GNUC_PATCHLEVEL__;
}

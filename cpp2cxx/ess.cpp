
#include <string>
#include <tuple>

namespace concept {
class C {
 public:
 double attempt();
 double accept();
 void reject();
};
}

namespace CTINT { 

struct A {

 /// doc of A
 int i = 3;

 double x = 2;
 double r = 2;
 double t = 2;
 double g = 2;
 double h = 2;
 double ru = 2;
 double xxx = 2;

 double zzzz;
};

template <typename T> struct B {

 /// doc of A
 int ik = 3;

 std::string x;

 T y;
};

}

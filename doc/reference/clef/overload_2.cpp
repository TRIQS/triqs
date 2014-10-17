#include <triqs/clef.hpp>

struct Obj {
 double v;                  // put something in it
 Obj(double v_) : v(v_) {}  // constructor
 Obj(Obj const &) = delete; // a non copyable object, to illustrate that we do NOT copy...

 // The "normal", non CLEF call operator ....
 double operator()(double x) const { return 10 * x; }

 // This macro implements properly an overload of the operator ()
 TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

 // a method
 double my_method(double x) const { return 2 * x; }

 // CLEF overload
 // WARNING : the method MUST be const
 TRIQS_CLEF_IMPLEMENT_LAZY_METHOD(Obj, my_method);

 // Just to print itself nicely in the expressions
 friend std::ostream &operator<<(std::ostream &out, Obj const &x) { return out << "Obj"; }
};

int main() {
 Obj f(7);
 triqs::clef::placeholder<1> x_;
 triqs::clef::placeholder<2> y_;

 std::cout << "Clef expression     : " << f(y_) + 2 * x_ << std::endl;
 std::cout << "Complete evaluation : " << eval(f(x_) + 2 * x_, x_ = 1) << std::endl;
 std::cout << "Partial evaluation  : " << eval(f(y_) + 2 * x_, y_ = 1) << std::endl;
 std::cout << "Complete evalution  : " << eval(f(y_) + 2 * x_, x_ = 3, y_ = 1) << std::endl << std::endl;

 std::cout << "Clef expression     : " << f.my_method(y_) + 2 * x_ << std::endl;
 std::cout << "Complete evaluation : " << eval(f.my_method(x_) + 2 * x_, x_ = 1) << std::endl;
 std::cout << "Partial evaluation  : " << eval(f.my_method(y_) + 2 * x_, y_ = 1) << std::endl;
 std::cout << "Complete evalution  : " << eval(f.my_method(y_) + 2 * x_, x_ = 3, y_ = 1) << std::endl;
}


from wrap_generator import *

# The module
module = module_(full_name = "my_module", doc = " Doc of my_module ")
module.add_include("<triqs/../test/pytriqs/wrap_test/a.hpp>") 
module.add_include("<triqs/arrays.hpp>")
module.add_include("<triqs/python_tools/converters/string.hpp>")
module.add_include("<triqs/python_tools/converters/arrays.hpp>")
module.add_include("<triqs/python_tools/converters/h5.hpp>")
module.add_include("<triqs/python_tools/converters/vector.hpp>")
module.add_include("<triqs/python_tools/converters/function.hpp>")
module.add_include("<triqs/python_tools/converters/gf.hpp>")
module.add_include("<triqs/python_tools/converters/map.hpp>")

# one class
g = class_(
        py_type = "Ac",
        c_type = "A",
        #serializable= "boost",
        serializable= "tuple",
        is_printable= True,
        hdf5 = True,
        arithmetic = ("algebra","double")
        )

# add a constructor
g.add_constructor(signature="()", doc = "DOC of constructor")

# add a method m1, with 3 overloads in C++ : dispatch is done on the type of the arguments 
g.add_method(name = "m1", c_name = "m1", signature = "double (int u, double y = 3)", doc = "DOC of m1")
g.add_method(name = "m1", c_name = "m1", signature = "double (int u)", doc = "DOC of m1...")
g.add_method(name = "m1", c_name = "m2", signature = "double (double u)", doc = "DOC of m1...")

# another version of the method, with some pre/post processing written in python

def ffg( *args, **kw) : 
    """ my doc of ffg in module """
    print "calling ffg, with :"
    print args
    print kw
    return tuple(2*x for x in args), kw

def post1(res) :
    #print "calling post1 inline"
    return [res]

# First version uses the external module version, second inline
#g.add_method(name = "m1p", c_name = "m1", signature = "double (int u, double y = 3)", doc = "DOC of mm", python_precall = "aux.ffg", python_postcall = "aux.post1")
g.add_method(name = "m1p", c_name = "m1", signature = "double (int u, double y = 3)", doc = "DOC of mm", python_precall = ffg, python_postcall = post1)

# demo of adding a simple piece of C++ code, there is no C++ method corresponding
g.add_method(name = "m1_x", calling_pattern = "bool result = (self_c.x >0) && (self_c.x < 10)" , signature = "bool()", doc = "A method which did not exist in C++")

#
g.add_method(name = "sm", c_name = "sm", signature = "int (int u)", is_static = True, doc = "a static method")

# alternative syntax 
#g.add_method(name = "m1", python_precall = "aux.ffg", python_postcall = "aux.post1").add_overload(c_name = "m1", rtype = "double", doc = "DOC of mm", args = [("int","u"), ("double","y",3)])

# older syntax, giving rtype and args (better for automatic scripts).
g.add_method(name = "m1f", c_name = "m1", signature = "double(int u, double y=3)", doc = "DOC of mm")

g.add_method(name = "long_fnt", c_name = "long_fnt", signature = "void()", release_GIL_and_enable_signal = True)
g.add_member(c_name = "count", c_type = "int",read_only=True)

# add the call operator 
g.add_call(signature = "int(int u)", doc = "call op")

# add getitem/setitem ...
g.add_getitem(signature = "double(int i)", doc = " doc [] ")
g.add_setitem(signature = "void(int i, double v)", doc = " doc [] set ")

# We can also add the call to a pure python function !
g.add_pure_python_method("pytriqs.wrap_test.aux.pure_py1")

def ffg2(self, *args, **kw) : 
    """ my doc of the function ffg2 """
    print "calling ffg2 [inline], with :"
    print args
    print kw
    #return [2*x for x in args], kw
    print dir()
    return tuple(2*x for x in args), kw

g.add_pure_python_method(ffg2)

# public members -> as properties
g.add_member(c_name = "x", c_type = "double", doc = "x field of A ....")
g.add_member(c_name = "y", c_type = "double", doc = "y field of A : read_only", read_only=True)

# properties : transform a couple of methods into properties
g.add_property(name = "i", getter = cfunction(c_name="_get_i", doc = "i prop get doc", signature = "int()"),
                           setter = cfunction(c_name="_set_i", doc = "i prop set doc", signature = "void(int j)"))

g.add_property(name = "ii", getter = cfunction(c_name="_get_i", doc = "i prop get doc", signature = "int()"))

g.add_iterator()

module.add_class(g)

# various module functions....
module.add_function (name = "print_a", signature = "void(A a)", doc = "DOC of print_a")
module.add_function (name = "print_err", signature = "void(A a)", doc = "DOC of print_a")
module.add_function ("std::vector<int> make_vector(int size)", doc = "DOC of print_a")
module.add_function (name = "make_vector2", signature = "std::vector<std::vector<int>>(int size)", doc = "DOC ....")
module.add_function (name = "vector_x2", signature = "std::vector<int>(std::vector<int> v)", doc = "DOC of print_a")

module.add_function (name = "make_matrix", signature = "matrix_view<double>(int size)", doc = "DOC ....")

module.add_function (name = "iter_on_range", signature = "void (range r)" , doc = "DOC ....")

module.add_function (name = "make_fnt_ii", signature = {'c_name': 'make_fnt_ii', 'rtype': "std::function<int(int,int)>", 'args': []}, doc = "....")
module.add_function (name = "make_fnt_iid", signature = {'c_name': 'make_fnt_iid', 'rtype': "std::function<int(int,int,double)>", 'args': []}, doc = "....")
module.add_function (name = "make_fnt_void", signature = {'c_name': 'make_fnt_void', 'rtype': "std::function<void(int,int)>", 'args': []}, doc = "....")

module.add_function (name = "use_fnt_ii", signature = "void(std::function<int(int,int)> f)", doc = "....")
module.add_function (name = "use_fnt_iid", signature = "void(std::function<int(int,int,double)> f)", doc = "....")

module.add_function (name = "map_to_mapvec", signature = "std::map<std::string,std::vector<int>>(std::map<std::string,int> m)", doc = "DOC of print_map")

def f1(x,y):
    print " I am in f1 ", x,y
    print y + 1/0.2
    print tuple([x])
    assert x>0, "an horrible error"

# The code of f1 will be copied verbatim in the module, and compiled at the
# initialisation of the module
module.add_python_function(f1)

if __name__ == '__main__' : 
   module.generate_code()


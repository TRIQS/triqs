from wrap_generator import *

# The operator class
op = class_(
        py_type = "Operator",
        c_type = "many_body_operator<double>",
        is_printable= True,
        arithmetic = ("algebra","double")
        )

op.add_constructor(signature="()", doc="create zero operator")

# Complete the number_protocol
mbo = 'many_body_operator<double>'
scal = 'double'

# Allow + and - between scalar and operator
add = op.number_protocol['add']
add.add_overload (calling_pattern = "+", args = [(mbo,'x'), (scal,'y')], rtype = mbo)
add.add_overload (calling_pattern = "+", args = [(scal,'x'), (mbo,'y')], rtype = mbo)
sub = op.number_protocol['subtract']
sub.add_overload (calling_pattern = "-", args = [(mbo,'x'), (scal,'y')], rtype = mbo)
sub.add_overload (calling_pattern = "-", args = [(scal,'x'), (mbo,'y')], rtype = mbo)

# Allow unary - on an operator
neg = pyfunction(py_name = "__neg__")
neg.arity = 1
neg.add_overload (calling_pattern = "-", args = [(mbo,'x')], rtype = mbo)
op.number_protocol['negative'] = neg

# The many_body_operators module
module = module_(full_name = "operators2", doc = "Doc of my_module")
module.add_include("<triqs/operators/many_body_operator.hpp>")
module.add_include("<triqs/arrays.hpp>")
module.add_using("namespace triqs::utility")
module.add_class(op)

# Annihilation operators
module.add_function(name = "c", signature="many_body_operator<double>(std::string ind1)", doc="annihilation operator")
module.add_function(name = "c", signature="many_body_operator<double>(std::string ind1, std::string ind2)", doc="annihilation operator")
module.add_function(name = "c", signature="many_body_operator<double>(std::string ind1, int i)", doc="annihilation operator")
module.add_function(name = "c", signature="many_body_operator<double>(int i, std::string ind1)", doc="annihilation operator")
module.add_function(name = "c", signature="many_body_operator<double>(int i, int j)", doc="annihilation operator")

# Construction operators
module.add_function(name = "c_dag", signature="many_body_operator<double>(std::string ind1)", doc="annihilation operator")
module.add_function(name = "c_dag", signature="many_body_operator<double>(std::string ind1, std::string ind2)", doc="annihilation operator")
module.add_function(name = "c_dag", signature="many_body_operator<double>(std::string ind1, int i)", doc="annihilation operator")
module.add_function(name = "c_dag", signature="many_body_operator<double>(int i, std::string ind1)", doc="annihilation operator")
module.add_function(name = "c_dag", signature="many_body_operator<double>(int i, int j)", doc="annihilation operator")

# to generate the module code
if __name__ == '__main__' : 
   module.generate_code(mako_template = sys.argv[1], wrap_file = sys.argv[2])
   module.generate_py_converter_header(mako_template = sys.argv[3], wrap_file = sys.argv[4])


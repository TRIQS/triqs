
Cpp2py Changelog :
 - remove the need to use variables of wrapped types by pointer
   e.g. when wrapping f( AWrappedType a)
   in C code, use a, not *a ...
 - Simpler, less error prone.

 - impl : just define a variable __x for parsing and then x is the user variable.

- Eliminate py_converters.hpp files
- All the information in the module .so itself.

- Partial rewrite of the parsing part.
  I now use directly the libclang AST,
  suppressing the intermediate representation that used in version 1.
  Code is shorter, simpler in some cases, more standard.

- introduce a small config.py for Cmake configured var

- rm notion of view in cpp2py

- Removing precall, postcall.



ViewValue pattern in TRIQS
##################################

* for array, matrix, vector, tail, gf of any kind.
* always of the same form.

* A value class with value semantics
* A view class to take partial views & slices, with view semantics.

How to implement ?

* X_impl<View>
  --> X and X_view by derivation : reimplement only construction (PATTERN) and =.

* Rational : 
  do NOT derive X from X_view !



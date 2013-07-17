
Copy policy in building expressions [Advanced]
====================================================

When building expressions, a problem can appear for the object at the leaves of the expressions tree
(scalar, placeholders, various callable objects, etc...).

The question is whether they should be captured by value or by reference.

In the lazy library, the choice has been made to capture them **by value**.

This means that, by default, *all objects appearing in a lazy expression are copied, rather than captured by reference*.

This is necessary to store expressions (with auto like above) for future reuse, transform them into new expressions
(e.g. make partial evaluation). Expressions are objects. 
Capturing the leaves by reference easily produces dangling references.
[Basically, an expression is a function of its placeholder. The leaves are the parameters of the function, 
they must leave as long as the expression live. In other words, we need to have a closure of the function].

The drawback of this approach is that it can generate unless copies of large objects.
There are several solutions to this issue : 

* The object is very small (like a double), hence making a copy in not a problem.
* You *know* that the object `A` will survive the expression, so using a reference is not a problem.
  You can use the `lazy(A)` expression that will wrap the reference to the object.
* The object has a compagnon view object (like array, array_view). In this case, 
  one wishes to put a view of the object rather than a copy in the expression.
  There are two cases.

  * TRIQS objects like array, matrix, vector will do it automatically.

  * For another object, if the object defines the tag `has_view_type_tag` as ::

     typedef void has_view_type_tag;
     typedef MY_VIEW_TYPE view_type;
  
    the instance of the object will be replaced by an instance of its view_type in building the expression.

For an illustration, Cf....


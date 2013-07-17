 
FAQ
======

Should I use array_view or array & as a return_type of a function ?
----------------------------------------------------------------------

It depends...

* array & is slightly quicker, since creating a view means copying the index systems (lengths and strides) 
  and a shared_ptr.
* BUT array_view will keep the reference counting.

So except in very critical parts of the code, the recommendation is to return a view.





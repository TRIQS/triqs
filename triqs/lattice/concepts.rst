

Concept Function 
return_type
arg_type
return_type operator()(arg_type const &) const  : the evaluation of the function


Concept FunctionOnBravaisLattice

refines Function
bravais_lattice const & lattice() const 


Concept ShortRangeFunctionOnBravaisLattice

* refines FunctionOnBravaisLattice
* derived from Tag::ShortRangeFunctionOnBravaisLattice  ??
* map_type
* begin, end: iterator -> pair<arg_type, return_type>


Concept FunctionOnBrillouinZone

* refines Function
brillouin_zone const & bz() const




Hamiltonians
============

.. highlight:: python

A set of python functions to construct of standard Hamiltonians is provided in the module ``pytriqs.operators.hamiltonians``.
Namely, three standard interacting local Hamiltonians are implemented: Slater, (Hubbard-)Kanamori and density-density Hamiltonians.
The Hamiltonians can be written for any set of spin and orbital names. Note that there is no actual notion of spin, the spin names
simply identify a division of the GF structure into non-hybridizing blocks.

In all the functions below, the following keywords have a standard form and meaning:

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
spin_names                  list of strings              Names of the spins, e.g. ['up','down']
orb_names                   list of strings or int       Names of the orbitals, e.g. [0,1,2] or ['t2g','eg']
off_diag                    boolean                      Do the GF blocks have (orbital) off-diagonal elements?
map_operator_structure      dict                         Mapping of names of GF blocks names from one convention to another, 
                                                         e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}
H_dump                      string                       Name of the file to which the Hamiltonian should be written
=========================   ===========================  ===========================================================================


Slater Hamiltonian
^^^^^^^^^^^^^^^^^^

This Hamiltonian contains the fully rotationally-invariant 4-index interactions:

.. math::
   H = \sum_{ijkl,\sigma \sigma'} U_{ijkl} a_{i \sigma}^\dagger a_{j \sigma'}^\dagger a_{l \sigma'} a_{k \sigma}.

.. function:: h_loc_slater(spin_names,orb_names,U_matrix,off_diag=None,map_operator_structure=None,H_dump=None)

    Returns the Hamiltonian as an Operator.
    The interaction parameters are:

=========================   ===========================  ===============================================
Name                        Type                         Meaning
=========================   ===========================  ===============================================
U_matrix                    4D matrix or array           U_{ijkl}
=========================   ===========================  ===============================================


Hubbard-Kanamori Hamiltonian
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This Hamiltonian contains the density-density, spin-fip and pair-hopping interactions:

.. math::
   H = \frac{1}{2} \sum_{(i \sigma) \neq (j \sigma')} U_{i j}^{\sigma \sigma'} n_{i \sigma} n_{j \sigma'}
        - \sum_{i \neq j} J a^\dagger_{i \uparrow} a_{i \downarrow} a^\dagger_{j \downarrow} a_{j \uparrow}
        + \sum_{i \neq j} J a^\dagger_{i \uparrow} a^\dagger_{i \downarrow} a_{j \downarrow} a_{j \uparrow}.

.. function:: h_loc_kanamori(spin_names,orb_names,U,Uprime,J_hund,off_diag=None,map_operator_structure=None,H_dump=None)

    Returns the Hamiltonian as an Operator.
    The interaction parameters are:

=========================   ===========================  ===============================================
Name                        Type                         Meaning
=========================   ===========================  ===============================================
U                           2D matrix or array           U_{ij}^{\sigma \sigma} (same spins)
Uprime                      2D matrix or array           U_{ij}^{\sigma \bar{\sigma}} (opposite spins)
J_hund                      scalar                       J
=========================   ===========================  ===============================================


Density-density Hamiltonian
^^^^^^^^^^^^^^^^^^^^^^^^^^^

This Hamiltonian contains only the density-density interactions:

.. math::
   H = \frac{1}{2} \sum_{(i \sigma) \neq (j \sigma')} U_{i j}^{\sigma \sigma'} n_{i \sigma} n_{j \sigma'}.

.. function:: h_loc_density(spin_names,orb_names,U,Uprime,off_diag=None,map_operator_structure=None,H_dump=None)

    Returns the Hamiltonian as an Operator.
    The interaction parameters are:

=========================   ===========================  ===============================================
Name                        Type                         Meaning
=========================   ===========================  ===============================================
U                           2D matrix or array           U_{ij}^{\sigma \sigma} (same spins)
Uprime                      2D matrix or array           U_{ij}^{\sigma \bar{\sigma}} (opposite spins)
=========================   ===========================  ===============================================


Helper functions
^^^^^^^^^^^^^^^^

The following helper functions are also present:

.. function:: get_mkind(off_diag,map_operator_structure)

   Set function that makes index for operators and GF blocks for a given spin 
   and orbital name.
* If off-diagonal blocks are present (off_diag is True), the operators and blocks are denoted by ``('spin', 'orbital')``.
* In the absence of off-diagonal blocks (off_diag is False), the operators and blocks are denoted by ``('spin_orbital',0)``.
* If map_operator_structure is given, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
 
.. function:: set_operator_structure(spin_names,orb_names,off_diag)

   Set the operator_structure (i.e. gf_struct) for given spin and orbital names,
   according to whether or not the Green's functions contain off-diagonal blocks.


.. seealso::

    Tools to construct the U matrices above are provided in the ``U_matrix`` module of 
    the `dft_tools <http://ipht.cea.fr/triqs/1.2/applications/dft_tools/>`_ application.

.. index:: wien2k_inst

.. _wien2k_inst:

Complete the installation of Wien2TRIQS (all platforms)
-------------------------------------------------------------

If you choose to install the Wien2TRIQS interface to the Wien2K package for doing DMFT calculations of  
realistic systems, you need take the last steps manually since Wien2k installation is not standard on all machines.
During TRIQS installation several files will be will be put into ::
  
   path_to_TRIQS_install_directory/share/triqs/Wien2k_SRC_files/SRC_templates
 
Those files are:: 
 * :file:`case.cf_f_mm2`  and :file:`case.cf_p_cubic` containing matrices for the complex->cubic transformation of the local angular basis
    
 * :file:`case.indmftpr` is a template for the input file needed by the :program:`dmftproj` program. This program constructs a set of localized orbitals representing correlated states.

These files then have to be copied manually to :file:`path_to_Wien2k/SRC_templates`, where :file:`path_to_Wien2k` is the path to Wien2K main directory. 

When building the Wien2k extension module, TRIQS will compile :program:`dmftproj` and install it into :file:`path_to_TRIQS_install_directory/bin`. 

In addition, :file:`path_to_Wien2k/SRC_templates` also contains :program:`run_triqs` and :program:`runsp_triqs` scripts for running Wien2k+DMFT fully self-consistent calculations. These files should be copied to :file:`path_to_Wien2k`. 

You will also need to insert manually a correct call of  :file:`pytriqs` into these scripts using an appropriate for your system MPI wrapper (mpirun, mpprun...), if needed. Search for *pytriqs* within the scripts to locate the appropriate place for inserting the :file:`pytriqs` call.

Finally, you will have to change the calls to :program:`python_with_DMFT` to :program:`pytriqs` in the Wien2k :file:`path_to_Wien2k/run*` files.

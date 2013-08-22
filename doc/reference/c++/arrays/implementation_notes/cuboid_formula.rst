
.. _cuboid_formula:

Cuboid formula
======================

* **Notations** :

   * R = rank 
   * index : (i0,...,i_{R-1})
   * Lengths : (L0, ... , L_{R-1})
   * Strides : (S0, ... , S_{R-1})
   * position = start_shift  + \sum_{j=0}^{R-1} i_j S_j

* **Strides for compact cuboid** : 

 If :math:`\sigma(i)` is the i-th index in memory (0 the first, R-1 the last one), we have 

 .. math::
   :nowrap:

   \begin{align*} 
   S_{\sigma(0)}  &= 1\\
   S_{\sigma(1)} &= L_{\sigma(0)} S_{\sigma(0)} = L_{\sigma(0)}\\
   S_{\sigma(i)} &= L_{\sigma(i-1)} S_{\sigma(i-1)} =  \prod_{j=i-1}^{0} L_{\sigma(j)}
   \end{align*} 


* **Slicing the cuboid** : 


 * Argument of the slice : (R0, R1, ..., R_{R-1}), with R_i = range : (start,end,step)
   for the moment, if R_i = E_i, a number, consider it as a special range of length 1.
 * new index (k0,...., k_{R-1}).
   i_u = R_u.start + k_u * R_u.step 
 * Compute the new strides : Sn_j

  .. math::
   :nowrap:

   \def\offset{\text{offset}}
   \begin{align*} 
     position &= \offset  + \sum_{j=0}^{R-1} i_j S_j \\
            &= \offset  +  \sum_{j=0}^{R-1} (k_j* R_j.step  + R_j.start) S_j \\
            &= (\offset + \sum_{j=0}^{R-1} (k_j* R_j.step  + R_j.start) S_j )  +  \sum_{j=0}^{R-1} k_j (S_j * R_j.start)
    \\
    \offset_\text{new} &= \offset + \sum_{j=0}^{R-1} (k_j* R_j.step  + R_j.start) S_j \\
                      &= position( R_j.start) \\
    Sn_j &= S_j * R_j.start
   \end{align*} 

  * Now : for the cases R_i = E_i, just drop the index.









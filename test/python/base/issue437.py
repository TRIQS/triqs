# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Olivier Parcollet, Nils Wentzell

from triqs.gf import *                                             
                                                                           
G_iw = GfImFreq(beta=40,indices=[0,1])                                     
G_w = GfReFreq(window=(-3,3),indices=[0,1])                                
                                                                                                          
A_iw = G_iw.copy()
A_w =G_w.copy()                                            
A_iw << 1.0j*G_iw                                                           
A_w << 1.0j*G_w

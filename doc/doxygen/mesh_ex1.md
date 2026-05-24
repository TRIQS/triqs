@page mesh_ex1 Example 1: Mesh types

[TOC]

This example shows how to construct each of the mesh types provided by @ref triqs-meshes and how to iterate their 
points. 
Each section below is a self-contained `main()` whose source lives in `doc/doxygen/examples/` and is built by 
`doc/doxygen/examples/CMakeLists.txt`, so it cannot drift out of sync with the library.

For the equivalent Python usage, see the Python user guide.

@section mesh_ex1_brzone Brillouin-zone mesh

@include brzone.cpp

Output:

@code{.text}
mesh point #0: index = [0, 0, 0], data index = 0, value = [0.0000, 0.0000, 0.0000]
mesh point #1: index = [0, 0, 1], data index = 1, value = [0.0000, 0.0000, 0.3333]
mesh point #2: index = [0, 0, 2], data index = 2, value = [0.0000, 0.0000, 0.6667]
mesh point #3: index = [0, 1, 0], data index = 3, value = [0.0000, 0.5000, 0.0000]
mesh point #4: index = [0, 1, 1], data index = 4, value = [0.0000, 0.5000, 0.3333]
mesh point #5: index = [0, 1, 2], data index = 5, value = [0.0000, 0.5000, 0.6667]
mesh point #6: index = [1, 0, 0], data index = 6, value = [0.5000, 0.0000, 0.0000]
mesh point #7: index = [1, 0, 1], data index = 7, value = [0.5000, 0.0000, 0.3333]
mesh point #8: index = [1, 0, 2], data index = 8, value = [0.5000, 0.0000, 0.6667]
mesh point #9: index = [1, 1, 0], data index = 9, value = [0.5000, 0.5000, 0.0000]
mesh point #10: index = [1, 1, 1], data index = 10, value = [0.5000, 0.5000, 0.3333]
mesh point #11: index = [1, 1, 2], data index = 11, value = [0.5000, 0.5000, 0.6667]
@endcode

@section mesh_ex1_chebyshev Chebyshev mesh

@include chebyshev.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = 0.24471741852423234
mesh point #1: index = 1, data index = 1, value = 2.061073738537635
mesh point #2: index = 2, data index = 2, value = 5
mesh point #3: index = 3, data index = 3, value = 7.938926261462366
mesh point #4: index = 4, data index = 4, value = 9.755282581475768
@endcode

@section mesh_ex1_cyclat Cyclic-lattice mesh

@include cyclat.cpp

Output:

@code{.text}
mesh point #0: index = [0, 0, 0], data index = 0, value = [0, 0, 0]
mesh point #1: index = [0, 0, 1], data index = 1, value = [0, 0, 0.5]
mesh point #2: index = [0, 0, 2], data index = 2, value = [0, 0, 1]
mesh point #3: index = [0, 1, 0], data index = 3, value = [0, 0.5, 0]
mesh point #4: index = [0, 1, 1], data index = 4, value = [0, 0.5, 0.5]
mesh point #5: index = [0, 1, 2], data index = 5, value = [0, 0.5, 1]
mesh point #6: index = [1, 0, 0], data index = 6, value = [0.5, 0, 0]
mesh point #7: index = [1, 0, 1], data index = 7, value = [0.5, 0, 0.5]
mesh point #8: index = [1, 0, 2], data index = 8, value = [0.5, 0, 1]
mesh point #9: index = [1, 1, 0], data index = 9, value = [0.5, 0.5, 0]
mesh point #10: index = [1, 1, 1], data index = 10, value = [0.5, 0.5, 0.5]
mesh point #11: index = [1, 1, 2], data index = 11, value = [0.5, 0.5, 1]
@endcode

@section mesh_ex1_discrete Discrete mesh

@include discrete.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0
mesh point #1: index = 1, data index = 1
mesh point #2: index = 2, data index = 2
mesh point #3: index = 3, data index = 3
mesh point #4: index = 4, data index = 4
@endcode

@section mesh_ex1_dlr DLR mesh

@include dlr.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = -4.997323654048254
mesh point #1: index = 1, data index = 1, value = -3.831753911537679
mesh point #2: index = 2, data index = 2, value = -2.710662984621819
mesh point #3: index = 3, data index = 3, value = -1.5985695686131243
mesh point #4: index = 4, data index = 4, value = 0.0013381729758728256
mesh point #5: index = 5, data index = 5, value = 2.075899665814476
mesh point #6: index = 6, data index = 6, value = 3.831753911537679
mesh point #7: index = 7, data index = 7, value = 4.997323654048254
@endcode

@section mesh_ex1_dlr_imfreq DLR imaginary-frequency mesh

@include dlr_imfreq.cpp

Output:

@code{.text}
mesh point #0: index = -6, data index = 0, value = -3.455751918948772i
mesh point #1: index = -3, data index = 1, value = -1.5707963267948966i
mesh point #2: index = -2, data index = 2, value = -0.9424777960769379i
mesh point #3: index = -1, data index = 3, value = -0.3141592653589793i
mesh point #4: index = 0, data index = 4, value = 0.3141592653589793i
mesh point #5: index = 1, data index = 5, value = 0.9424777960769379i
mesh point #6: index = 2, data index = 6, value = 1.5707963267948966i
mesh point #7: index = 5, data index = 7, value = 3.455751918948772i
@endcode

@section mesh_ex1_dlr_imtime DLR imaginary-time mesh

@include dlr_imtime.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = 0.012031950007446723
mesh point #1: index = 1, data index = 1, value = 0.6496895210536141
mesh point #2: index = 2, data index = 2, value = 2.0222028313159592
mesh point #3: index = 3, data index = 3, value = 5.283961182488998
mesh point #4: index = 4, data index = 4, value = 7.022202831315959
mesh point #5: index = 5, data index = 5, value = 8.584483769065113
mesh point #6: index = 6, data index = 6, value = 9.550004964934757
mesh point #7: index = 7, data index = 7, value = 9.987968049992553
@endcode

@section mesh_ex1_imfreq Imaginary-frequency mesh

@include imfreq.cpp

Output:

@code{.text}
mesh point #0: index = -3, data index = 0, value = -1.5707963267948966i
mesh point #1: index = -2, data index = 1, value = -0.9424777960769379i
mesh point #2: index = -1, data index = 2, value = -0.3141592653589793i
mesh point #3: index = 0, data index = 3, value = 0.3141592653589793i
mesh point #4: index = 1, data index = 4, value = 0.9424777960769379i
mesh point #5: index = 2, data index = 5, value = 1.5707963267948966i
@endcode

@section mesh_ex1_imtime Imaginary-time mesh

@include imtime.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = 0
mesh point #1: index = 1, data index = 1, value = 2.5
mesh point #2: index = 2, data index = 2, value = 5
mesh point #3: index = 3, data index = 3, value = 7.5
mesh point #4: index = 4, data index = 4, value = 10
@endcode

@section mesh_ex1_legendre Legendre mesh

@include legendre.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0
mesh point #1: index = 1, data index = 1
mesh point #2: index = 2, data index = 2
mesh point #3: index = 3, data index = 3
mesh point #4: index = 4, data index = 4
@endcode

@section mesh_ex1_prod Product mesh

@include prod.cpp

Output:

@code{.text}
mesh point #0: index = ([0, 0, 0], -1), data index = (0, 0)
mesh point #1: index = ([0, 0, 0], 0), data index = (0, 1)
mesh point #2: index = ([0, 0, 0], 1), data index = (0, 2)
mesh point #3: index = ([0, 1, 0], -1), data index = (1, 0)
mesh point #4: index = ([0, 1, 0], 0), data index = (1, 1)
mesh point #5: index = ([0, 1, 0], 1), data index = (1, 2)
mesh point #6: index = ([1, 0, 0], -1), data index = (2, 0)
mesh point #7: index = ([1, 0, 0], 0), data index = (2, 1)
mesh point #8: index = ([1, 0, 0], 1), data index = (2, 2)
mesh point #9: index = ([1, 1, 0], -1), data index = (3, 0)
mesh point #10: index = ([1, 1, 0], 0), data index = (3, 1)
mesh point #11: index = ([1, 1, 0], 1), data index = (3, 2)
@endcode

@section mesh_ex1_refreq Real-frequency mesh

@include refreq.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = -5
mesh point #1: index = 1, data index = 1, value = -2.5
mesh point #2: index = 2, data index = 2, value = 0
mesh point #3: index = 3, data index = 3, value = 2.5
mesh point #4: index = 4, data index = 4, value = 5
@endcode

@section mesh_ex1_refreq_log Logarithmic real-frequency mesh

@include refreq_log.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = -10
mesh point #1: index = 1, data index = 1, value = -5
mesh point #2: index = 2, data index = 2, value = -2.5
mesh point #3: index = 3, data index = 3, value = -1.25
mesh point #4: index = 4, data index = 4, value = -0.625
mesh point #5: index = 5, data index = 5, value = -0.3125
mesh point #6: index = 6, data index = 6, value = -0.15625
mesh point #7: index = 7, data index = 7, value = 0.15625
mesh point #8: index = 8, data index = 8, value = 0.3125
mesh point #9: index = 9, data index = 9, value = 0.625
mesh point #10: index = 10, data index = 10, value = 1.25
mesh point #11: index = 11, data index = 11, value = 2.5
mesh point #12: index = 12, data index = 12, value = 5
mesh point #13: index = 13, data index = 13, value = 10
@endcode

@section mesh_ex1_refreq_pts Custom-point real-frequency mesh

@include refreq_pts.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = -5
mesh point #1: index = 1, data index = 1, value = -1
mesh point #2: index = 2, data index = 2, value = 0
mesh point #3: index = 3, data index = 3, value = 1
mesh point #4: index = 4, data index = 4, value = 5
@endcode

@section mesh_ex1_retime Real-time mesh

@include retime.cpp

Output:

@code{.text}
mesh point #0: index = 0, data index = 0, value = -5
mesh point #1: index = 1, data index = 1, value = -2.5
mesh point #2: index = 2, data index = 2, value = 0
mesh point #3: index = 3, data index = 3, value = 2.5
mesh point #4: index = 4, data index = 4, value = 5
@endcode

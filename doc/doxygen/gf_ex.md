@page gf_ex Green's functions

[TOC]

The @ref triqs-gfs "gf" class is the central container of TRIQS. A Green's function
`gf<Mesh, Target>` couples a @ref triqs-meshes "mesh" (the domain — real or imaginary time, real or
imaginary frequency, Legendre, DLR, Brillouin zone, ...) with a *target* (the value at each mesh
point — a scalar, a matrix, or a higher-rank tensor). The same container therefore represents
@f$G(\tau)@f$, @f$G(i\omega_n)@f$, @f$G(t)@f$, @f$G(\omega)@f$, ... with a uniform interface.

The examples below show how to construct these objects, fill them, and evaluate them. Each section
is a self-contained `main()` whose source lives in `doc/doxygen/examples/` and is compiled by
`doc/doxygen/examples/CMakeLists.txt`, so the snippets cannot drift out of sync with the library.

For the equivalent Python usage, see the Python user guide and the `triqs.gfs` API documentation.

@section gf_ex_matsub Matsubara Green's functions

A single-variable Matsubara Green's function lives on a `mesh::imfreq` (fermionic or bosonic). Here
we construct @f$G(i\omega) = \frac{1}{i\omega - 3}@f$ by filling it from a placeholder expression and
then read back a value. The mesh, target shape and statistic are fixed at construction.

@include gf_tut_matsub_1.cpp

Output:

@code{.text}
[[(-0.158986,-0.16649)]]

[[(-0.158986,-0.16649)]]
@endcode

Green's functions can also depend on several arguments. The next example builds a two-frequency
object @f$G(i\omega, i\nu) = \frac{1}{i\omega + i\nu - 4}@f$ on a product of two Matsubara meshes,
illustrating how `gf` generalizes to multi-variable domains.

@include gf_tut_matsub_2.cpp

Output:

@code{.text}
[[(-0.0721001,-0.113255)]]

[[(-0.0721001,-0.113255)]]
@endcode

@section gf_ex_imtime Imaginary-time Green's functions

The imaginary-time counterpart @f$G(\tau)@f$ lives on a `mesh::imtime`. The construction mirrors the
Matsubara case — only the mesh type changes — which is the whole point of the generic `gf` container.

@include gf_tutorial_3.cpp

@section gf_ex_retime Real-time Green's functions

A real-time Green's function @f$G(t)@f$ is defined on a `mesh::retime` spanning `[t_min, t_max]`. The
*target* determines the value stored at each time. For a scalar-valued @f$G(t)@f$:

@include gf_tutorial_0.cpp

For a matrix-valued target of size `n x m` (e.g. a multi-orbital propagator):

@include gf_tutorial_1.cpp

The target can have any rank — here a three-index tensor:

@include gf_tutorial_2.cpp

@section gf_ex_two_time Two-time Green's functions

Just like the two-frequency example above, a real-time object can depend on two times
@f$G(t, t')@f$ by using a product mesh:

@include gf_tutorial_4.cpp

@section gf_ex_placeholders Filling a GF with placeholders

TRIQS Green's functions integrate with the CLEF placeholder mini-language: assigning a lazy
expression in a placeholder fills every mesh point without writing an explicit loop. This is the
idiomatic way to initialize a `gf` from a closed-form expression.

@include gf_tutorial_5.cpp

@section gf_ex_interp Interpolating the GF at an arbitrary point

Calling a Green's function with parentheses *evaluates* it at an arbitrary point of its domain using
the interpolation rule the mesh declares (linear in imaginary time, exact in Matsubara, etc.) — as
opposed to bracket access `[]`, which looks up an existing mesh point.

@include gf_tutorial_6.cpp

Output:

@code{.text}
(0.48,0)
@endcode

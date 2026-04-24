#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_meshes_GUARDS
#define C2PY_HXX_DECLARATION_meshes_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::mesh::imtime>           = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::imtime>       = "triqs.mesh.meshes.MeshImTime";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::energy_t>         = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::energy_t>     = "triqs.mesh.meshes.EnergyT";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::imfreq>           = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::imfreq>       = "triqs.mesh.meshes.MeshImFreq";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::dlr>              = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::dlr>          = "triqs.mesh.meshes.MeshDLR";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::dlr_imtime>       = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::dlr_imtime>   = "triqs.mesh.meshes.MeshDLRImTime";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::dlr_imfreq>       = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::dlr_imfreq>   = "triqs.mesh.meshes.MeshDLRImFreq";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::retime>           = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::retime>       = "triqs.mesh.meshes.MeshReTime";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::refreq>           = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::refreq>       = "triqs.mesh.meshes.MeshReFreq";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::cyclat>           = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::cyclat>       = "triqs.mesh.meshes.MeshCycLat";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::brzone>           = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::brzone>       = "triqs.mesh.meshes.MeshBrZone";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::chebyshev>        = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::chebyshev>    = "triqs.mesh.meshes.MeshChebyshev";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::fourier_poly>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::fourier_poly> = "triqs.mesh.meshes.MeshFourierPoly";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::legendre>         = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::legendre>     = "triqs.mesh.meshes.MeshLegendre";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::refreq_log>       = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::refreq_log>   = "triqs.mesh.meshes.MeshReFreqLog";
template <> constexpr bool c2py::is_wrapped<triqs::mesh::refreq_pts>       = true;
template <> inline constexpr auto c2py::tp_name<triqs::mesh::refreq_pts>   = "triqs.mesh.meshes.MeshReFreqPts";
#endif
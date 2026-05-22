#include <c2py/c2py.hpp>
#include <nda/nda.hpp>
#include <nda/c2py/converters.hpp>
#include <triqs/c2py_converters/gf.hpp>
#include <triqs/lattice/bravais_lattice.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/lattice/tight_binding.hpp>
#include <triqs/mesh.hpp>

namespace c2py {

  // Converter for the hopping dictionary used in the tight-binding constructor.
  template <> struct py_converter<triqs::lattice::hopping_dict> {
    using c_t = triqs::lattice::hopping_dict;

    static constexpr const char *tp_name = "dict[tuple[int,...], ndarray]";

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyDict_Check(ob)) return true;
      if (raise_exception) PyErr_SetString(PyExc_TypeError, "Expected a dict for hopping_dict");
      return false;
    }

    static c_t py2c(PyObject *ob) {
      c_t result;
      PyObject *key, *value;
      Py_ssize_t pos = 0;
      while (PyDict_Next(ob, &pos, &key, &value)) {
        result.displ_vec.push_back(py2cxx<nda::vector<long>>(key));
        result.overlap_mat_vec.push_back(py2cxx<nda::matrix<std::complex<double>>>(value));
      }
      return result;
    }

    static PyObject *c2py(c_t const &x) {
      pyref d = PyDict_New();
      for (size_t i = 0; i < x.displ_vec.size(); ++i) {
        pyref k = cxx2py(x.displ_vec[i]);
        pyref v = cxx2py(x.overlap_mat_vec[i]);
        if (k.is_null() || v.is_null()) return nullptr;
        PyDict_SetItem(d, k, v);
      }
      return d.new_ref();
    }
  };

} // namespace c2py

namespace triqs::lattice {

  // explicit template instantiations
  template r_t bravais_lattice::lattice_to_real_coordinates(r_t const &) const;
  template k_t brillouin_zone::lattice_to_real_coordinates(k_t const &) const;
  template r_t tight_binding::lattice_to_real_coordinates(r_t const &) const;
  template auto tight_binding::dispersion(nda::vector_const_view<double> const &) const;
  template auto tight_binding::dispersion(nda::array_const_view<double, 2> const &) const;
  template auto tight_binding::fourier(nda::vector_const_view<double> const &) const;
  template auto tight_binding::fourier(nda::array_const_view<double, 2> const &) const;

} // namespace triqs::lattice

#include "lattice_tools.wrap.cxx"

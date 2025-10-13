#pragma once
#include <nda/nda.hpp>
#include <cassert>
#include "fourier_polynomial.hpp"

static constexpr auto r_all = nda::range::all;

namespace triqs {
  namespace tb {

    // FIXME updtate for FP changes!

    template <int kdim> class tb_velocity : public fourier_polynomial<3, kdim> {

      // -------------------------
      long norbitals;

      public:
      tb_velocity(std::vector<std::array<long, kdim>> Rs, std::vector<nda::array<dcomplex, 2>> hoppings,
                  std::vector<nda::array<dcomplex, 3>> position_op)
         : norbitals(hoppings[0].shape(0)) {

        // merge position_op and iR * hoppings, as
        // v[R, a, b, x] = i R[R, x] H[R, a, b] + r[R, a, b, x]
        // TODO is there a smarter way to take the product of Rs and hoppings along R index?
        std::vector<nda::array<dcomplex, 3>> vR = 1j * Rs * hoppings + position_op;
        fourier_polynomial<3, kdim>(Rs, vR);
      };

      //------------------ evaluation functions --------------------------------------

      // TODO: throw an error if any k value outside 0,1?
      // TODO convert this to some iterator over k

      // TODO this one is a bit challenging technically, so I wait to implement it
      /** Function to take a list of kpoints and return an array of energies
     * @param klist : list of kpoints in crystal / fractional coordinates (from [0,1) )
     * @return energies : vector of energies in shape [k, norbitals, norbitals]
     */
      /*     nda::matrix<double> vk_band_basis(std::vector<std::array<double, kdim>> kpoints) {

      // energies in the shape of nk, nbands
      nda::matrix<double> velocities(kpoints.size(), norbitals);
      for (auto [ik, k] : itertools::enumerate(kpoints)) {
        // returns with dimensions norbitals, norbitals
        energies(ik, r_all) = nda::linalg::eigenvalues(std::apply(this->operator(), k));
      }
      return velocities;
    }
 */
      /** Function to take a list of kpoints and return a matrix of of energies in orbital space
     * @param kpoints : list of kpoints in crystal / fractional coordinates (from [0,1) )
     * @return velocities : array of velocities of dimension (k, norb, norb, xyz)
     */
      nda::array<dcomplex, 4> vk_orbital_basis(std::vector<std::array<double, kdim>> const &kpoints) {

        // energies in the shape of nk, nbands
        nda::array<dcomplex, 4> velocities(kpoints.size(), norbitals, norbitals, kdim);
        for (auto [ik, k] : itertools::enumerate(kpoints)) {
          // returns something with dimensions norbitals, norbitals
          velocities(ik, r_all, r_all, r_all) = std::apply(*this, k);
        }
        return velocities;
      }

      // -------------------- print -------------------
      friend std::ostream &operator<<(std::ostream &sout, tb_velocity const &tb) {
        sout << "tb_velocity [";
        for (auto const &[displ, overlap_mat] : itertools::zip(tb.Rs, tb.coefficients)) sout << "\n   " << displ << " : " << overlap_mat;
        return sout << " ]";
      }
    };
  } // namespace tb
} // namespace triqs

#pragma once

#include <nda/nda.hpp>

#include <utility>

namespace triqs::experimental::utility {

  /**
   * @addtogroup triqs-experimental-utility
   * @{
   */

  /**
   * @brief Integrate a CLEF expression over one of its placeholders using an arbitrary one-dimensional integrator.
   *
   * @details This function turns the given expression into a unary CLEF function of the placeholder with index \f$ I
   * \f$ and hands it to the supplied one-dimensional integrator together with the integration domain. If the expression
   * still depends on other placeholders, the result is again a lazy CLEF expression; otherwise it is the value of the
   * integral.
   *
   * @tparam D Type of the integration domain (e.g. a pair of bounds).
   * @tparam I Index of the placeholder that is integrated over.
   * @param integrator1d One-dimensional integrator that integrates a callable \f$ f(x) \f$ over the domain.
   * @param expr_to_integrate CLEF expression to be integrated.
   * @param p Placeholder-to-domain assignment, i.e. a `placeholder = domain` pair.
   * @return Lazy CLEF expression if other placeholders remain, otherwise the value of the integral.
   */
  template <typename D, int I> auto integrate(auto const &integrator1d, auto expr_to_integrate, nda::clef::pair<I, D> const &p) {
    auto f = nda::clef::make_function(std::move(expr_to_integrate), nda::clef::placeholder<I>{});
    return integrator1d(std::move(f), p.rhs);
  }

  /** @} */

} // namespace triqs::experimental::utility

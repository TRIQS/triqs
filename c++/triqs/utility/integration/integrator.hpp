#pragma once
#include <nda/clef/placeholder.hpp>
#include <nda/clef/utils.hpp>
#include <nda/nda.hpp>

namespace triqs::utility {

  // -----------------------------
  /**
    * @brief Integrate an expression over a x_= domain, with any custom integrator
    *
    * @tparam D Domain type
    * @tparam I Placeholder index
    * @param integrator1d One dimension integrator, integrating a f(x) on a segment
    * @param expr_to_integrate The expression to integrate
    * @param p A pair placeholder = value, e.g. x_ = segment{...}
    * @return A lazy expression some placeholder are not integrated, else the value of the integral
    */
  template <typename D, int I> auto integrate(auto const &integrator1d, auto expr_to_integrate, nda::clef::pair<I, D> const &p) {
    auto f = nda::clef::make_function(std::move(expr_to_integrate), nda::clef::placeholder<I>{});
    return integrator1d(std::move(f), p.rhs);
  }

} // namespace triqs::utility

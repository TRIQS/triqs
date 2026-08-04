// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

// Fast SIMD sincos via Chebyshev-interpolation polynomial approximation on [-pi/4, pi/4].
// Coefficients computed with polyfit (https://github.com/DiamonDinoia/polyfit) using FuseAlways.
// Parity-split form: sin(x) ≈ x + x³·P(x²), cos(x) ≈ 1 + x²·Q(x²).
// TolDigits selects how many polynomial terms to keep (fewer = faster).

#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <numbers>
#include <tuple>
#include <type_traits>
#include <utility>

#include <xsimd/xsimd.hpp>

namespace triqs::utility::math {

namespace detail {

template <class T>
concept simd_batch = requires(const T &value) {
  typename T::value_type;
  T::size;
  xsimd::nearbyint(value);
};

template <class T>
concept angle_arg = std::floating_point<std::remove_cvref_t<T>> || simd_batch<std::remove_cvref_t<T>>;

template <class T> struct scalar_impl { using type = std::remove_cvref_t<T>; };
template <simd_batch T> struct scalar_impl<T> { using type = typename std::remove_cvref_t<T>::value_type; };
template <class T> using scalar_t = typename scalar_impl<std::remove_cvref_t<T>>::type;

struct consts {
  static constexpr double pi_over_2     = std::numbers::pi / 2.0;
  static constexpr double neg_pi_over_2 = -pi_over_2;
  static constexpr double inv_pi_over_2 = 1.0 / pi_over_2;
};

// ---- Full-precision minimax coefficients (polyfit, degree 14 sin / degree 13 cos) ----
// sin(x) ≈ x + x³ · horner(sin_inner, x²)   (Horner in x², highest power first)
// cos(x) ≈ 1 + x² · horner(cos_tail, x²)
// The trailing constant (~1.0 for sin's x coefficient, ~1.0 for cos's x^0) is
// NOT in these arrays — it is reconstructed as: fma(poly, x³, x) and fma(poly, x², 1).

inline constexpr std::array<double, 6> sin_inner_full = {
   0x1.5e585f68f956ep-33,  // u^5  (x^13)
  -0x1.ae5f687b275b3p-26,  // u^4  (x^11)
   0x1.71de33799ebc6p-19,  // u^3  (x^9)
  -0x1.a01a019367fdp-13,   // u^2  (x^7)
   0x1.1111111104f1dp-7,   // u^1  (x^5)
  -0x1.555555555541bp-3,   // u^0  (x^3)  ← must always be included
};

inline constexpr std::array<double, 6> cos_tail_full = {
   0x1.1b88ad1c62723p-29,  // u^5  (x^12)
  -0x1.27df3a1e26a95p-22,  // u^4  (x^10)
   0x1.a019f7fcecefp-16,   // u^3  (x^8)
  -0x1.6c16c163eaf27p-10,  // u^2  (x^6)
   0x1.555555554ef27p-5,   // u^1  (x^4)
  -0x1.fffffffffff91p-2,   // u^0  (x^2)  ← must always be included (-0.5)
};

// Compile-time truncation: return the last N elements (lowest-order, most important terms).
template <std::size_t N, std::size_t M> consteval std::array<double, N> tail(std::array<double, M> const &src) {
  static_assert(N <= M);
  std::array<double, N> out{};
  for (std::size_t i = 0; i < N; ++i) out[i] = src[M - N + i];
  return out;
}

// Map TolDigits → number of inner/tail polynomial terms.
// Error ≈ first_dropped_coeff · (π/4)^(2·dropped_power + 3) for sin,
//         first_dropped_coeff · (π/4)^(2·dropped_power + 2) for cos.
// Validated empirically: minimum N that passes all NFFT accuracy tests at each tolerance.
consteval std::size_t sin_nterms(int tol_digits) {
  if (tol_digits <= 6) return 2;
  if (tol_digits <= 8) return 3;
  if (tol_digits <= 10) return 4;
  return 6; // full precision
}
consteval std::size_t cos_nterms(int tol_digits) {
  if (tol_digits <= 6) return 2;
  if (tol_digits <= 8) return 3;
  if (tol_digits <= 10) return 4;
  return 6; // full precision
}

template <int TolDigits> inline constexpr auto sin_inner = tail<sin_nterms(TolDigits)>(sin_inner_full);
template <int TolDigits> inline constexpr auto cos_tail  = tail<cos_nterms(TolDigits)>(cos_tail_full);

// ---- Horner evaluation with FMA ----

template <class T, class U> [[gnu::always_inline]] inline auto fma_or_mul_add(T acc, T x, U c) {
  if constexpr (simd_batch<T>)
    return xsimd::fma(acc, x, T(c));
  else if constexpr (std::floating_point<T>)
    return std::fma(acc, x, static_cast<T>(c));
  else
    return acc * x + c;
}

template <class T, std::size_t N, std::size_t... I>
[[gnu::always_inline]] inline auto eval_horner_impl(std::array<double, N> const &coeffs, T const &x, std::index_sequence<I...>) {
  auto acc = std::remove_cvref_t<T>(coeffs[0]);
  ((acc = fma_or_mul_add(acc, x, coeffs[I + 1])), ...);
  return acc;
}

template <std::size_t N, class T>
[[gnu::always_inline]] inline auto eval_horner(std::array<double, N> const &coeffs, T const &x) {
  static_assert(N > 0);
  if constexpr (N == 1)
    return std::remove_cvref_t<T>(coeffs[0]);
  else
    return eval_horner_impl(coeffs, x, std::make_index_sequence<N - 1>{});
}

// ---- Reduced-angle evaluation: |t| <= pi/4 ----

template <int TolDigits, class X> [[gnu::always_inline]] inline auto evaluate_reduced(X const &t) {
  static constexpr auto si = sin_inner<TolDigits>;
  static constexpr auto ct = cos_tail<TolDigits>;
  auto const t2 = t * t;
  auto const t3 = t2 * t;
  auto const sp = eval_horner(si, t2);
  auto const cp = eval_horner(ct, t2);
  using real = scalar_t<X>;
  return std::pair{fma_or_mul_add(sp, t3, t), fma_or_mul_add(cp, t2, real(1))};
}

// ---- Angle reduction + quadrant adjustment ----

template <int TolDigits, angle_arg Angle> [[gnu::flatten]] auto sincos_impl(Angle const &angle) {
  using A    = std::remove_cvref_t<Angle>;
  using real = scalar_t<Angle>;

  if constexpr (simd_batch<A>) {
    auto const qf = xsimd::nearbyint(angle * A(consts::inv_pi_over_2));
    auto const x1 = xsimd::fma(qf, A(consts::neg_pi_over_2), angle);
    auto const q4 = xsimd::fma(A(real(-4)), xsimd::floor(qf * A(real(0.25))), qf);
    auto const [s1, c1]     = evaluate_reduced<TolDigits>(x1);
    auto const even_mask     = (q4 == A(real(0))) | (q4 == A(real(2)));
    auto const low_half_mask = q4 < A(real(2));
    auto const s2 = xsimd::select(even_mask, s1, c1);
    auto const c2 = xsimd::select(even_mask, c1, -s1);
    return std::tuple{xsimd::select(low_half_mask, s2, -s2), xsimd::select(low_half_mask, c2, -c2)};
  } else {
    auto const qi = static_cast<long long>(std::nearbyint(static_cast<real>(angle) * consts::inv_pi_over_2));
    real const x1 = [&] {
      if constexpr (std::is_same_v<real, float>) {
        constexpr real pio2_1 = 1.5703125f;
        constexpr real pio2_2 = 4.837512969970703125e-4f;
        constexpr real pio2_3 = 7.549789954891882e-8f;
        auto xr               = std::fma(static_cast<real>(qi), -pio2_1, static_cast<real>(angle));
        xr                    = std::fma(static_cast<real>(qi), -pio2_2, xr);
        return std::fma(static_cast<real>(qi), -pio2_3, xr);
      } else {
        return std::fma(static_cast<real>(qi), consts::neg_pi_over_2, static_cast<real>(angle));
      }
    }();
    auto const [s1, c1] = evaluate_reduced<TolDigits>(x1);
    real const s2 = (qi & 1) == 0 ? s1 : c1;
    real const c2 = (qi & 1) == 0 ? c1 : -s1;
    return std::tuple{(qi & 2) == 0 ? s2 : -s2, (qi & 2) == 0 ? c2 : -c2};
  }
}

} // namespace detail

/// Fast sincos with compile-time accuracy selection.
/// NOTE: the truncated buckets are lowest-order tails of the full-degree minimax
/// fit, so they behave like Taylor truncations and fall short of their nominal
/// labels: measured absolute errors are about 4e-4 (6), 4e-6 (8) and 3e-8 (10);
/// bucket 12 is at double-precision machine accuracy. Accuracy-critical seed
/// tables must use 12, since recurrence/ladder multiplies amplify the seed error
/// by the exponent magnitude.
template <int TolDigits = 12, detail::angle_arg Angle> auto sincos(Angle const &angle) {
  return detail::sincos_impl<TolDigits>(angle);
}

} // namespace triqs::utility::math

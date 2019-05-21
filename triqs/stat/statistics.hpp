/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by T. Ayral, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <type_traits>
#include <vector>
#include <cmath>
#include <boost/iterator/iterator_facade.hpp>

namespace triqs {
  namespace statistics {

    // trait to find out if T models the concept TimeSeries
    template <typename T> struct is_time_series : std::false_type {};
    template <typename T> struct is_time_series<T &> : is_time_series<T> {};
    template <typename T> struct is_time_series<T &&> : is_time_series<T> {};
    template <typename T> struct is_time_series<T const> : is_time_series<T> {};
    template <typename T> struct is_time_series<std::vector<T>> : std::true_type {};

    /* *********************************************************
  *
  *  Binning
  *
  * ********************************************************/

    template <typename ValueType> class binned_series {
      int bin_size;
      std::vector<ValueType> binned;

      public:
      using value_type = ValueType;

      template <typename TimeSeries>
      binned_series(TimeSeries const &t, int bin_size_) : bin_size(bin_size_), binned(t.size() / bin_size_, value_type{}) {
        if (bin_size_ > t.size())
          TRIQS_RUNTIME_ERROR << "bin size (" << bin_size_ << ") cannot be larger than size (" << t.size() << ") of time series";
        for (int i = 0; i < size(); i++) {
          for (int j = 0; j < bin_size; j++) binned[i] += t[i * bin_size + j];
          binned[i] /= bin_size;
        }
      }

      value_type operator[](int i) const { return binned[i]; }
      int size() const { return binned.size(); }

      std::vector<value_type> const &data() const & { return binned; }
      std::vector<value_type> const &data() & { return binned; }
      std::vector<value_type> data() && { return std::move(binned); }

      using const_iterator = typename std::vector<ValueType>::const_iterator;
      const_iterator begin() const { return binned.begin(); }
      const_iterator end() const { return binned.end(); }

      friend std::ostream &operator<<(std::ostream &out, binned_series const &s_) {
        for (auto const &x : s_.binned) out << x << " ";
        return out;
      }
    };

    template <typename T> struct is_time_series<binned_series<T>> : std::true_type {};

    /// Factory
    template <typename TimeSeries> binned_series<typename TimeSeries::value_type> make_binned_series(TimeSeries const &t, int bin_size) {
      return {t, bin_size};
    }

    /* *********************************************************
  *
  *  TS_observer: an implementation class
  *  Contains a ref or a value to a TS, and the implementation of the const_iterator
  *
  * ********************************************************/
    template <typename Derived, typename TimeSeries> class ts_observer { // TimeSeries can be a T or a T &
      protected:
      TimeSeries ts;

      public:
      using value_type = typename std::remove_reference<TimeSeries>::type::value_type;

      template <typename TS> ts_observer(TS &&t_) : ts(std::forward<TS>(t_)) {}

      ts_observer(ts_observer const &) = default;
      ts_observer(ts_observer &&)      = default;
      ts_observer &operator=(ts_observer const &) = delete;
      ts_observer &operator=(ts_observer &&) = default;

      int size() const { return ts.size(); }

      // const_iterator
      class const_iterator : public boost::iterator_facade<const_iterator, value_type, boost::forward_traversal_tag, value_type> {
        friend class boost::iterator_core_access;
        Derived const *t;
        int u;
        void increment() { ++u; }
        value_type dereference() const { return (*t)[u]; }
        bool equal(const_iterator const &other) const { return ((t == other.t) && (other.u == u)); }

        public:
        const_iterator(Derived const *m, bool at_end) : t(m) { u = (at_end ? m->size() : 0); }
      };

      const_iterator begin() const { return {static_cast<Derived const *>(this), false}; }
      const_iterator end() const { return {static_cast<Derived const *>(this), true}; }
      const_iterator cbegin() const { return begin(); }
      const_iterator cend() const { return end(); }

      // printing
      friend std::ostream &operator<<(std::ostream &out, ts_observer const &s_) {
        for (auto const &x : s_) out << x << "  ";
        return out;
      }
    };

    /* *********************************************************
  *
  *  Jackknife
  *
  * ********************************************************/
    template <typename TimeSeries> class jackknife : public ts_observer<jackknife<TimeSeries>, TimeSeries> {
      using B = ts_observer<jackknife<TimeSeries>, TimeSeries>;
      typename B::value_type sum;

      public:
      template <typename TS> jackknife(TS &&t_) : B(std::forward<TS>(t_)) {
        sum     = typename B::value_type{0 * this->ts[0]};
        auto si = this->ts.size();
        for (int i = 0; i < si; i++) sum += this->ts[i];
      }

      typename B::value_type operator[](int i) const { return (sum - this->ts[i]) / (this->size() - 1); }
    };

    ///
    template <typename TimeSeries> jackknife<TimeSeries> make_jackknife(TimeSeries &&t) { return {std::forward<TimeSeries>(t)}; }

    template <typename T> struct is_time_series<jackknife<T>> : std::true_type {};

    /* *********************************************************
  *
  *  Observable
  *
  * ********************************************************/

    template <typename T> class observable {
      std::vector<T> _series;

      public:
      observable() { _series.reserve(1000); }
      observable(std::vector<T> &&v) : _series(std::move(v)) {}
      observable(binned_series<T> &&s) : _series(std::move(s).data()) {}

      std::vector<T> const &get_series() const { return _series; }

      observable &operator<<(T x) { // copy and move : check speed ... or overload const &, &&
        _series.push_back(std::move(x));
        return *this;
      }
      template <typename A> observable &operator<<(A &&a) {
        _series.emplace_back(std::forward<A>(a));
        return *this;
      }

      // TimeSeries concept
      using value_type = T;
      int size() const { return _series.size(); }
      T operator[](int i) const { return _series[i]; }
    };
    template <typename T> struct is_time_series<observable<T>> : std::true_type {};

    /* *********************************************************
  *
  *  Expressions
  *
  * ********************************************************/

    // -------------- clef leaf evaluation ----------------------

    struct repl_by_jack {};

    struct bin_and_repl_by_jack {
      int bin_size;
    };

    template <typename T> auto eval(observable<T> const &obs, repl_by_jack) DECL_AND_RETURN(make_jackknife(obs));

    template <typename T>
    auto eval(observable<T> const &obs, bin_and_repl_by_jack info) DECL_AND_RETURN(make_jackknife(make_binned_series(obs), info.bin_size));

    template <typename TS> auto eval(TS const &obs, int i) -> std::enable_if_t<is_time_series<TS>::value, decltype(obs[i])> { return obs[i]; }

// --------- Operations --------------------------

// The principle is :
// All operations between a time_series and anything results in a clef lazy expression
// This implements the case of binary/unary operators when there is no clef expression (which is already handled by clef operators
// hence this avoid ambiguity).
#define TRIQS_TS_OPERATION(TAG, OP)                                                                                                                  \
  template <typename L, typename R>                                                                                                                  \
  std::enable_if_t<(is_time_series<L>::value || is_time_series<R>::value) && (!clef::is_any_lazy<L, R>::value),                                      \
                   clef::expr_node_t<clef::tags::TAG, L, R>>                                                                                         \
  operator OP(L &&l, R &&r) {                                                                                                                        \
    return {clef::tags::TAG(), std::forward<L>(l), std::forward<R>(r)};                                                                              \
  }

    TRIQS_TS_OPERATION(plus, +);
    TRIQS_TS_OPERATION(minus, -);
    TRIQS_TS_OPERATION(multiplies, *);
    TRIQS_TS_OPERATION(divides, /);
#undef TRIQS_TS_OPERATION

// Any function overloaded for clef should also accept object modelling is_time_series
// Here : define all math function defined for clef ...
#define AUX(r, data, elem) TRIQS_CLEF_EXTEND_FNT_LAZY(elem, is_time_series)
    BOOST_PP_SEQ_FOR_EACH(AUX, nil, TRIQS_CLEF_STD_MATH_FNT_TO_MAKE_LAZY);
#undef AUX

#undef TRIQS_TS_FUNCTION

    // -------------   Dress an expression as a time serie   --------------------------

    template <typename Expr> struct _immutable_time_series {
      Expr expr;
      using value_type = typename std::remove_reference<decltype(eval(expr, 0))>::type;
      value_type operator[](int i) const { return eval(expr, i); }
      int size() const { return get_size(expr); }
    };

    template <typename T> struct is_time_series<_immutable_time_series<T>> : std::true_type {};

    // make_immutable_time_series (x) returns :
    //  x if it is already a time_series
    //  _immutable_time_series(x) if it is an expression
    template <typename T> std::enable_if_t<clef::is_clef_expression<T>::value, _immutable_time_series<T>> make_immutable_time_series(T &&x) {
      return {std::forward<T>(x)};
    }
    template <typename T> std::enable_if_t<is_time_series<T>::value, T> make_immutable_time_series(T &&x) { return std::forward<T>(x); }

    // -------------  Computation of the size --------------------------

    // a function object that when called on x, returns nothing but :
    // if x models TimeSeries : check if its sizes is equal to previously encountered and stores it
    // otherwise do nothing
    struct _get_size_visitor {
      int res;
      template <typename T> std::enable_if_t<!is_time_series<T>::value> operator()(T const &) {}
      template <typename T> std::enable_if_t<is_time_series<T>::value> operator()(T const &obs) {
        int i = obs.size();
        if ((res * i != 0) && (res != i)) TRIQS_RUNTIME_ERROR << "Expression of time series with time mismatch";
        res = i; // keep the result
      }
    };

    template <typename T> int get_size(T const &x) {
      auto l = _get_size_visitor{0};
      clef::apply_on_each_leaf(l, x);
      return l.res;
    }

    // -------------  A trait to get the value_type of an expression of observables----------

    template <typename ObservableExpr> struct _get_value_type { using type = decltype(eval(std::declval<ObservableExpr>(), 0)); };
    template <typename ObservableExpr> using get_value_type = typename _get_value_type<ObservableExpr>::type;

    /* *********************************************************
  *
  *  Average and error
  *
  * ********************************************************/

    // -------------   A value and its error --------------------------

    template <typename T> struct value_and_error_bar {
      T value, error_bar; // error is variance : a T???? complex ??
      friend std::ostream &operator<<(std::ostream &out, value_and_error_bar const &ve) { return out << ve.value << " +/- " << ve.error_bar; }
    };

    // -------------   empirical average and variance --------------------------

    template <typename TimeSeries> typename TimeSeries::value_type empirical_average(TimeSeries const &t) {
      auto si = t.size();
      if (si == 0) return typename TimeSeries::value_type{};
      auto sum = t[0];
      for (int i = 1; i < si; ++i) sum += t[i];
      return sum / t.size();
    }

    ///
    template <typename TimeSeries> typename TimeSeries::value_type empirical_variance(TimeSeries const &t) {
      auto si = t.size();
      if (si == 0) return typename TimeSeries::value_type{};

      auto avg           = empirical_average(t);
      decltype(avg) sum2 = (t[0] - avg) * (t[0] - avg); // also valid if t[0] is an array e.g., i.e. no trivial contructor...
      for (int i = 1; i < si; ++i) sum2 += (t[i] - avg) * (t[i] - avg);
      return sum2 / t.size();
    }

    // -------------  Overload average for observables and expressions of observables --------------------------

    template <typename T> T average(observable<T> const &obs) { return empirical_average(obs); }

    template <typename ObservableExpr> std::enable_if_t<clef::is_clef_expression<ObservableExpr>::value, double> average(ObservableExpr const &obs) {
      return empirical_average(_immutable_time_series<ObservableExpr>{obs});
    }

    // -------------  Overload average and error for observables and expressions of observables --------------------------

    template <typename T> value_and_error_bar<typename T::value_type> empirical_average_and_error(T const &ts) {
      using std::sqrt;
      return {empirical_average(ts), sqrt((ts.size() - 1.0) * (empirical_variance(ts)))};
    }

    template <typename T> value_and_error_bar<T> average_and_error(observable<T> const &obs) {
      auto ts = make_jackknife(obs);
      return empirical_average_and_error(ts);
    }

    template <typename T> value_and_error_bar<T> average_and_error(observable<T> const &obs, int bin_size) {
      auto ts = make_jackknife(make_binned_series(obs, bin_size));
      return empirical_average_and_error(ts);
    }

    template <typename ObservableExpr>
    std::enable_if_t<clef::is_clef_expression<ObservableExpr>::value, value_and_error_bar<get_value_type<ObservableExpr>>>
    average_and_error(ObservableExpr const &obs) {
      auto expr_jack = eval(obs, repl_by_jack{}); // replace every TS leaf by a jacknifed version
      return empirical_average_and_error(make_immutable_time_series(expr_jack));
    }

    template <typename ObservableExpr>
    std::enable_if_t<clef::is_clef_expression<ObservableExpr>::value, value_and_error_bar<get_value_type<ObservableExpr>>>
    average_and_error(ObservableExpr const &obs, int bin_size) {
      auto expr_bin_jack = eval(obs, bin_and_repl_by_jack{bin_size});
      return empirical_average_and_error(make_immutable_time_series(expr_bin_jack));
    }

    /* *********************************************************
  *
  *  Auto-correlations
  *
  * ********************************************************/

    // ------  k->  (<f(a(k))f(a(0))> - <f(a)>^2 )/ (<f(a)^2> - <f(a)>^2 ) --------------------
    template <typename TimeSeries> class normalized_autocorrelation : public ts_observer<normalized_autocorrelation<TimeSeries>, TimeSeries> {
      using B = ts_observer<normalized_autocorrelation<TimeSeries>, TimeSeries>;
      typename B::value_type var, avg;

      public:
      template <typename TS> normalized_autocorrelation(TS &&t_) : B(std::forward<TS>(t_)) {
        var = empirical_variance(this->ts);
        avg = empirical_average(this->ts);
      }

      typename B::value_type operator[](int k) const {
        const int N = this->size();
        auto r      = typename B::value_type{0 * this->ts[0]};
        for (int i = 0; i < N - k; i++) r += (this->ts[i + k] - avg) * (this->ts[i] - avg);
        r = (r / (N - k)) / var;
        return r;
      }
    };

    ///
    template <typename TimeSeries> normalized_autocorrelation<TimeSeries> make_normalized_autocorrelation(TimeSeries &&t) {
      return {std::forward<TimeSeries>(t)};
    }

    // ------  Auto-correlation time from the computation of the autocorrelation --------------------

    template <typename TimeSeries> int autocorrelation_time(TimeSeries const &a) {
      auto normalized_autocorr = make_normalized_autocorrelation(make_immutable_time_series(a)); // is a N*dim_f matrix...
      double t_int             = normalized_autocorr[0];                                         // in principle, a vector dim_f
      double coeff_tau         = 6;                                                              // if exponential decay -> 0.25 % precision
      for (int l_max = 1; l_max < coeff_tau * t_int; l_max++) t_int += normalized_autocorr[l_max];
      return int(t_int);
    }

    // ------  Auto-correlation time from binning --------------------

    template <typename TimeSeries> double autocorrelation_time_from_binning(TimeSeries const &A, double intrinsic_variance, double precision = 0.05) {

      auto size = make_immutable_time_series(A).size();

      auto t_cor = [&](int b) {
        auto A_binned = make_binned_series(make_immutable_time_series(A), b);
        using std::abs;
        double var = abs(empirical_variance(A_binned));
        return 0.5 * (b * var / intrinsic_variance - 1.);
      };

      double coeff = -6 * std::log(precision); // heuristic -> 0.25 % precision
      int B        = 2;
      while (B < coeff * std::abs(t_cor(B))) ++B;

      // now, B is large enough: average over a few estimates
      int Navg             = std::min(100, int(size - B));
      double autocorr_time = 0.;
      for (int b = 0; b < Navg; b++) autocorr_time += t_cor(B + b);
      return autocorr_time / Navg;
    }

    // ------  Auto-correlation time from binning --------------------

    template <typename TimeSeries> double autocorrelation_time_from_binning2(TimeSeries const &A) {

      auto size   = make_immutable_time_series(A).size();
      double var1 = empirical_variance(make_immutable_time_series(A));

      auto t_cor = [&](int b) {
        auto A_binned = make_binned_series(make_immutable_time_series(A), b);
        double var    = empirical_variance(A_binned);
        return .5 * var / var1 * b;
      };

      int B                 = 2;
      double autocorr_time  = t_cor(B);
      double slope          = 1.;
      int small_slope_count = 0;
      std::vector<double> t;
      while (small_slope_count < 100 && B < size / 10) {
        double t_cor_new = t_cor(++B);
        slope            = (std::abs(t_cor_new - autocorr_time) / autocorr_time);
        if (slope < 1e-5) small_slope_count++;
        if (small_slope_count > 0) t.push_back(t_cor_new);
        autocorr_time = t_cor_new;
      }
      return empirical_average(t);
    }

    template <typename TimeSeries> double autocorrelation_time_from_binning(TimeSeries const &A) {
      using std::abs;
      auto size   = make_immutable_time_series(A).size();
      double var1 = abs(empirical_variance(make_immutable_time_series(A)));

      int B   = 2;
      auto Ab = make_binned_series(A, 2);

      using std::abs;
      auto t_cor = [&Ab](int bin_size, double var1) {
        double var = abs(empirical_variance(Ab));
        return .5 * (var / var1 * bin_size - 1);
      };

      double autocorr_time  = t_cor(B, var1);
      double slope          = 1.;
      int small_slope_count = 0;
      std::vector<double> t;
      while (small_slope_count < 5 && B < size / 10) {
        B *= 2;
        Ab               = make_binned_series(Ab, 2);
        double t_cor_new = t_cor(B, var1);
        slope            = (std::abs(t_cor_new - autocorr_time) / autocorr_time);
        if (slope < .5 * 1e-1) small_slope_count++;
        if (small_slope_count > 0) t.push_back(t_cor_new);
        autocorr_time = t_cor_new;
        //std::cout << B << "\t" << t_cor_new << "\t" << slope << std::endl;
      }
      if (t.size() > 0) {
        return abs(empirical_average(t));
      } else {
        TRIQS_RUNTIME_ERROR << "autocorrelation time not converged!!";
      }
    }
  } // namespace statistics
} // namespace triqs

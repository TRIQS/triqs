#include <iterator>
#include <tuple>
#include <type_traits>
#include <vector>

#include <iostream>

namespace details {

template <class Iter> struct iterator_facade {

 Iter& self() { return static_cast<Iter&>(*this); }
 Iter const& self() const { return static_cast<const Iter&>(*this); }

 public:
 Iter& operator++() {
  self().next();
  return self();
 }

 Iter operator++(int) {
  Iter c = *this;
  self().next();
  return c;
 }

 template <typename U> bool operator==(U const& other) const { return self().equal(other); }
 template <typename U> bool operator!=(U const& other) const { return (!operator==(other)); }

 decltype(auto) operator*() const { return self().dereference(); }
 decltype(auto) operator-> () const { return operator*(); }
};

template <typename It> struct prod_iter_sentinel_t { It it; };
template <typename It> prod_iter_sentinel_t<It> make_prod_iter_sentinel(It&& it) { return {it}; }

template <typename... It> struct prod_iter : iterator_facade<prod_iter<It...>> {

 std::tuple<It...> its_b, its_e, its;
 prod_iter(std::tuple<It...> b, std::tuple<It...> e) : its_b(std::move(b)), its_e(std::move(e)), its(its_b) {}

 // using value_type = std::pair<long, std::decay_t<decltype(*std::declval<It>())>>;
 // using pointer = value_type*;
 // using reference = value_type&;
 using iterator_category = std::forward_iterator_tag;

 template <int N> void _next() {
  ++std::get<N>(its);
  if constexpr (N < sizeof...(It) - 1) {
   if (std::get<N>(its) == std::get<N>(its_e)) {
   std::get<N>(its) = std::get<N>(its_b);
   _next<N + 1>();
   }
  }
 }
 void next() { _next<0>(); }

 bool equal(prod_iter const& other) const { return (its == other.its); }
 template <typename U> bool equal(prod_iter_sentinel_t<U> const& s) const { return (s.it == std::get<sizeof...(It) - 1>(its)); }


 using seq_t = std::make_index_sequence<sizeof...(It)>;

 template <size_t... Is> auto tie_map_impl(std::index_sequence<Is...>) const { return std::tie(*std::get<Is>(its)...); }

 std::tuple<typename It::reference...> dereference() const { return tie_map_impl(seq_t{}); }
};

template <typename... It> prod_iter<std::decay_t<It>...> make_prod_iter(std::tuple<It...>&& x, std::tuple<It...>&& y) {
 return {std::move(x), std::move(y)};
}

template <typename... T> struct prod_ed {
 std::tuple<T...> tu; // T can be a ref.
 using seq_t = std::make_index_sequence<sizeof...(T)>;

 template <size_t... Is> auto _begin(std::index_sequence<Is...>) {
  return make_prod_iter(std::make_tuple(std::begin(std::get<Is>(tu))...), std::make_tuple(std::end(std::get<Is>(tu))...));
 }

 auto begin() { return _begin(seq_t{}); }
 auto end() { return make_prod_iter_sentinel(std::cend(std::get<sizeof...(T) - 1>(tu))); }
};


} // namespace details

template <typename... T> details::prod_ed<T...> prod(T&&... x) { return {std::tie(std::forward<T>(x)...)}; }



int f1(std::vector<int> const& v) {
 int r = 0;
 for (auto[x, y] : prod(v, v)) { r = r + y + x; }
 return r;
}

int f2(std::vector<int> const& v) {
 int r = 0;
 for (auto const& x : v) {
  for (auto const& y : v) { r = r + y + x; }
 }
 return r;
}

int f3(std::vector<int> const& v) {
 int r = 0;
 auto b1 = v.begin();
 auto b2 = v.begin();
 auto e1 = v.end();
 auto e2 = v.end();
 auto i1 = b1, i2 = b2;
 
 while (i2 != e2) { 
   ++i1;
   if (i1 == e1) {
       i1 = b1;
       ++i2;
r = r + y + x; 
   }
 }
 return r;
}


int main() {

 std::vector<int> v{0, 1, 2, 3};

 for (auto[x, y] : prod(v, v)) std::cout << x << " " << y << "\n";
}

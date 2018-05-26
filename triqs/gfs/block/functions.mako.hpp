#pragma once

namespace triqs {
  namespace gfs {

    /*mako
  %for FUNC in ['fourier']:
   %for ROOT in ['block', 'block2']:
    %for EXT in ['_gf<V, T> const&','_gf<V, T>&', '_gf_view<V, T>', '_gf_const_view<V, T>']:
    <%
      GF = ROOT + EXT
     %>
  */
    template <typename V, typename T> auto MAKO_FUNC(MAKO_GF g) {
      return make_lazy_transform([](auto &&x) { return MAKO_FUNC(x); }, g);
    };

    // mako %endfor
    // mako %endfor
    // mako %endfor

    // FIXME c++17
    //#define MAP_BGF_17(FUNC)                                                                                                         \
 //template <typename G> requires(is_block_gf_or_view<G>()) auto FUNC(G const& g) {                                                \
  //lazy_transform_t{[](auto&& x) { return FUNC(x); }, g};                                                                         \
 //}
    // MAP_BGF(fourier);

  } // namespace gfs
} // namespace triqs

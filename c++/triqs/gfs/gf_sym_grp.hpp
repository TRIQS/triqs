#pragma once
#include <triqs/gfs.hpp>
#include <nda/nda.hpp>
#include <nda/sym_grp.hpp>

namespace triqs {
  namespace gfs {

    // symmetry concept for scalar valued gf: symmetry accepts mesh index and returns new mesh index & operation
    template <typename F, typename G, typename mesh_idx_t = typename G::mesh_t::idx_t>
    concept ScalarGfSymmetry = is_gf_v<G> and //
       requires(F f, mesh_idx_t const &mesh_idx) {
         requires(G::target_t::rank == 0);
         { f(mesh_idx) } -> std::same_as<std::tuple<mesh_idx_t, nda::operation>>;
       };

    // symmetry concept for tensor valued gf: symmetry accepts mesh + target index and returns new mesh + target index & operation
    template <typename F, typename G, typename mesh_idx_t = typename G::mesh_t::idx_t,
              typename target_idx_t = std::array<long, static_cast<std::size_t>(G::target_t::rank)>>
    concept TensorGfSymmetry = is_gf_v<G> and //
       requires(F f, mesh_idx_t const &mesh_idx, target_idx_t const &target_idx) {
         requires(G::target_t::rank > 0);
         { f(mesh_idx, target_idx) } -> std::same_as<std::tuple<mesh_idx_t, target_idx_t, nda::operation>>;
       };

    // init function concept for scalar valued gf: init function accepts mesh index and returns gf value type
    template <typename F, typename G, typename mesh_idx_t = typename G::mesh_t::idx_t>
    concept ScalarGfInitFunc = is_gf_v<G> and //
       requires(F f, mesh_idx_t const &mesh_idx) {
         requires(G::target_t::rank == 0);
         { f(mesh_idx) } -> std::same_as<typename G::scalar_t>;
       };

    // init function concept for tensor valued gf: init function accepts mesh index + target index and returns gf value type
    template <typename F, typename G, typename mesh_idx_t = typename G::mesh_t::idx_t,
              typename target_idx_t = std::array<long, static_cast<std::size_t>(G::target_t::rank)>>
    concept TensorGfInitFunc = is_gf_v<G> and //
       requires(F f, mesh_idx_t const &mesh_idx, target_idx_t const &target_idx) {
         requires(G::target_t::rank > 0);
         { f(mesh_idx, target_idx) } -> std::same_as<typename G::scalar_t>;
       };

    // make tuple from array
    template <typename arr_t> auto make_tuple_from_array(arr_t const &arr) {
      constexpr auto fetch = [](auto const &...xs) { return std::tuple{xs...}; };
      return std::apply(fetch, arr);
    }

    // make tuple from array using index_sequence
    template <typename arr_t, std::size_t... Is> auto make_tuple_from_array(arr_t const &arr, std::index_sequence<Is...>) {
      return std::tuple{arr[Is]...};
    }

    // make array from tuple
    template <typename tuple_t> auto make_array_from_tuple(tuple_t const &tuple) {
      constexpr auto fetch = [](auto const &...xs) { return std::array{xs...}; };
      return std::apply(fetch, tuple);
    }

    template <typename F, typename G>
      requires(is_gf_v<G> && (ScalarGfSymmetry<F, G> || TensorGfSymmetry<F, G>))
    class sym_grp {

      private:
      // data aliases
      using data_t           = typename G::data_t;
      using value_t          = typename G::scalar_t;
      using data_idx_t       = std::array<long, static_cast<std::size_t>(nda::get_rank<data_t>)>;
      using data_sym_func_t  = std::function<std::tuple<data_idx_t, nda::operation>(data_idx_t const &)>;
      using data_init_func_t = std::function<value_t(data_idx_t const &)>;

      // mesh aliases
      using mesh_idx_t                = typename G::mesh_t::idx_t;
      static constexpr auto mesh_rank = n_variables<typename G::mesh_t>;

      // target aliases
      using target_t                    = typename G::target_t;
      static constexpr auto target_rank = target_t::rank;
      using target_idx_t                = std::array<long, static_cast<std::size_t>(target_rank)>;

      // members
      nda::sym_grp<data_sym_func_t, data_t> data_sym_grp; // symmetry group instance for the data array

      // convert from gf to nda symmetry
      data_sym_func_t to_data_symmetry(F const &f, G const &g) const {

        auto fp = [f, m = g.mesh()](data_idx_t const &x) {
          // init new data index and residual operation
          data_idx_t xp;
          nda::operation op;

          // scalar valued gfs
          if constexpr (target_rank == 0) {

            if constexpr (mesh_rank == 1) {
              auto [new_mesh_idx, opp] = f(m.to_idx(x[0]));
              xp[0]                    = m.to_datidx(new_mesh_idx);
              op                       = opp;

            } else {
              auto [new_mesh_idx, opp] = f(m.to_idx(make_tuple_from_array(x)));
              xp                       = make_array_from_tuple(m.to_datidx(new_mesh_idx));
              op                       = opp;
            }

            // tensor valued gfs
          } else {

            // convert data index to target index
            target_idx_t target_idx;
            for (auto i : range(target_rank)) target_idx[i] = x[i + mesh_rank];

            if constexpr (mesh_rank == 1) {
              // evaluate symmetry
              auto [new_mesh_idx, new_target_idx, opp] = f(m.to_idx(x[0]), target_idx);

              // convert mesh index + target index back to data index
              xp[0] = m.to_datidx(new_mesh_idx);
              for (auto i : range(target_rank)) xp[i + mesh_rank] = new_target_idx[i];
              op = opp;

            } else {
              // evaluate symmetry
              auto [new_mesh_idx, new_target_idx, opp] = f(m.to_idx(make_tuple_from_array(x, std::make_index_sequence<mesh_rank>{})), target_idx);

              // convert mesh index + target index back to data index
              auto new_mesh_arr = make_array_from_tuple(m.to_datidx(new_mesh_idx));
              for (auto i : range(mesh_rank)) xp[i] = new_mesh_arr[i];
              for (auto i : range(target_rank)) xp[i + mesh_rank] = new_target_idx[i];
              op = opp;
            }
          }

          return std::tuple{xp, op};
        };

        return fp;
      };

      // convert from list of gf to list of nda symmetry
      std::vector<data_sym_func_t> to_data_symmetry_list(G const &g, std::vector<F> const &sym_list) const {
        std::vector<data_sym_func_t> data_sym_list;
        for (auto f : sym_list) data_sym_list.push_back(to_data_symmetry(f, g));
        return data_sym_list;
      }

      // convert from gf to nda init function
      template <typename H> data_init_func_t to_data_init_func(G const &g, H const &h) const {

        auto hp = [h, m = g.mesh()](data_idx_t const &x) {
          // scalar valued gfs
          if constexpr (target_rank == 0) {

            if constexpr (mesh_rank == 1) {
              return h(m.to_idx(x[0]));

            } else {
              return h(m.to_idx(make_tuple_from_array(x)));
            }

            // tensor valued gfs
          } else {

            target_idx_t target_idx;
            for (auto i : range(target_rank)) target_idx[i] = x[i + mesh_rank];

            if constexpr (mesh_rank == 1) {
              return h(m.to_idx(x[0]), target_idx);

            } else {
              return h(m.to_idx(make_tuple_from_array(x, std::make_index_sequence<mesh_rank>{})), target_idx);
            }
          }
        };

        return hp;
      };

      public:
      [[nodiscard]] nda::sym_grp<data_sym_func_t, data_t> const &get_data_sym_grp() const { return data_sym_grp; }
      long num_classes() const { return data_sym_grp.num_classes(); }

      sym_grp() = default;
      sym_grp(G const &g, std::vector<F> const &sym_list, long const max_length = 0)
         : data_sym_grp{g.data(), to_data_symmetry_list(g, sym_list), max_length} {};

      // initializer method
      template <typename H>
      void init(G &g, H const &h, bool parallel = false) const
        requires(ScalarGfInitFunc<H, G> || TensorGfInitFunc<H, G>)
      {
        data_sym_grp.init(g.data(), to_data_init_func(g, h), parallel);
      }

      // symmetrization method
      std::tuple<double, mesh_idx_t, target_idx_t> symmetrize(G &g) const {
        auto const &[max_diff, max_idx] = data_sym_grp.symmetrize(g.data());
        auto const m                    = g.mesh();

        // scalar valued gfs
        if constexpr (target_rank == 0) {

          if constexpr (mesh_rank == 1) {
            return std::tuple{max_diff, m.to_idx(max_idx[0]), target_idx_t{}};

          } else {
            return std::tuple{max_diff, m.to_idx(make_tuple_from_array(max_idx)), target_idx_t{}};
          }

          // tensor valued gfs
        } else {

          // convert data index to target index
          target_idx_t target_idx;
          for (auto i : range(target_rank)) target_idx[i] = max_idx[i + mesh_rank];

          if constexpr (mesh_rank == 1) {
            return std::tuple{max_diff, m.to_idx(max_idx[0]), target_idx};

          } else {
            return std::tuple{max_diff, m.to_idx(make_tuple_from_array(max_idx, std::make_index_sequence<mesh_rank>{})), target_idx};
          }
        }
      }
    };
  } // namespace gfs
} // namespace triqs
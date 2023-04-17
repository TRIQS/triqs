#pragma once
#include <nda/sym_grp.hpp>

namespace triqs {
  namespace gfs {
    //------------------------------------------------------------------------------------------------------
    // concepts

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

    //------------------------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------------------------
    // helper functions

    // make tuple from array
    template <typename arr_t> auto make_tuple_from_array(arr_t const &arr) {
      auto fetch = [](auto const &...xs) { return std::tuple{xs...}; };
      return std::apply(fetch, arr);
    }

    template <typename arr_t, std::size_t... Is> auto make_tuple_from_array(arr_t const &arr, std::index_sequence<Is...>) {
      return std::tuple{arr[Is]...};
    }

    // make array from tuple
    template <typename tuple_t> auto make_array_from_tuple(tuple_t const &tuple) {
      auto fetch = [](auto const &...xs) { return std::array{xs...}; };
      return std::apply(fetch, tuple);
    }

    //------------------------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------------------------

    // symmetry group implementation
    template <typename F, typename G>
      requires(is_gf_v<G> && (ScalarGfSymmetry<F, G> || TensorGfSymmetry<F, G>))
    class sym_grp {

      private:
      // data aliases
      using data_t          = typename G::data_t;
      using data_idx_t      = std::array<long, static_cast<std::size_t>(get_rank<data_t>)>;
      using data_sym_func_t = std::function<std::tuple<data_idx_t, nda::operation>(data_idx_t const &)>;

      // mesh aliases
      using mesh_idx_t                = typename G::mesh_t::idx_t;
      static constexpr auto mesh_rank = n_variables<typename G::mesh_t>;

      // target aliases
      using target_t                    = typename G::target_t;
      static constexpr auto target_rank = target_t::rank;
      using target_idx_t                = std::array<long, static_cast<std::size_t>(target_rank)>;

      // members
      std::vector<F> sym_list;                            // list of symmetries defining the symmetry group
      nda::sym_grp<data_sym_func_t, data_t> data_sym_grp; // list of symmetric elements for the data array

      // convert from gf to nda symmetry
      data_sym_func_t to_data_symmetry(F const &f, G const &g) {

        auto fp = [f, m = g.mesh()](data_idx_t const &x) {
          // init new data index and residual operation
          data_idx_t xp;
          nda::operation op;

          // case 1: scalar valued gfs
          if constexpr (target_rank == 0) {

            if constexpr (mesh_rank == 1) {
              auto mesh_idx            = m.to_idx(x[0]);
              auto [new_mesh_idx, opp] = f(mesh_idx);
              xp[0]                    = m.to_datidx(new_mesh_idx);
              op                       = opp;

            } else {
              auto mesh_idx            = m.to_idx(make_tuple_from_array(x));
              auto [new_mesh_idx, opp] = f(mesh_idx);
              xp                       = make_array_from_tuple(m.to_datidx(new_mesh_idx));
              op                       = opp;
            }

            // case 2 : tensor valued gfs
          } else {

            if constexpr (mesh_rank == 1) {
              // convert data index to mesh index + target index
              auto mesh_idx = m.to_idx(x[0]);
              target_idx_t target_idx;
              for (auto i : range(target_rank)) target_idx[i] = x[i + mesh_rank];

              // evaluate symmetry
              auto [new_mesh_idx, new_target_idx, opp] = f(mesh_idx, target_idx);

              // convert mesh index + target index back to data index
              xp[0] = m.to_datidx(new_mesh_idx);
              for (auto i : range(target_rank)) xp[i + mesh_rank] = new_target_idx[i];
              op = opp;

            } else {
              // convert data index to mesh index + target index
              auto mesh_idx = m.to_idx(make_tuple_from_array(x, std::make_index_sequence<mesh_rank>{}));
              target_idx_t target_idx;
              for (auto i : range(target_rank)) target_idx[i] = x[i + mesh_rank];

              // evaluate symmetry
              auto [new_mesh_idx, new_target_idx, opp] = f(mesh_idx, target_idx);

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
      std::vector<data_sym_func_t> to_data_symmetry_list(std::vector<F> const &sym_list, G const &g) {
        std::vector<data_sym_func_t> data_sym_list;
        for (auto f : sym_list) data_sym_list.push_back(to_data_symmetry(f, g));
        return data_sym_list;
      }

      public:
      // getter methods (no setter methods, members should not be modified)
      [[nodiscard]] std::vector<F> const &get_sym_list() const { return sym_list; }
      [[nodiscard]] nda::sym_grp<data_sym_func_t, data_t> const &get_data_sym_grp() const { return data_sym_grp; }
      long num_classes() const { return data_sym_grp.get_sym_classes().size(); }

      // constructor
      sym_grp(std::vector<F> const &sym_list_, G const &g) : sym_list(sym_list_), data_sym_grp{g.data(), to_data_symmetry_list(sym_list, g)} {
        double speedup = g.data().size() / num_classes();
        std::cout << "Symmetry reduction completed, expected speedup: " << speedup << std::endl;
      };
    };

    //------------------------------------------------------------------------------------------------------

  } // namespace gfs
} // namespace triqs
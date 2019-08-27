
// common to many classes

// ------------- All the call operators arguments -----------------------------

template <typename... Args> decltype(auto) operator()(Args &&... args) const & {
  if constexpr (sizeof...(Args) == 0)
    return const_view_type{*this};
  else {
    static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>::value)) // any argument is lazy ?
      return clef::make_expr_call(*this, std::forward<Args>(args)...);
    else
      return evaluator_t()(*this, std::forward<Args>(args)...);
  }
}
template <typename... Args> decltype(auto) operator()(Args &&... args) & {
  if constexpr (sizeof...(Args) == 0)
    return view_type{*this};
  else {
    static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>::value)) // any argument is lazy ?
      return clef::make_expr_call(*this, std::forward<Args>(args)...);
    else
      return evaluator_t()(*this, std::forward<Args>(args)...);
  }
}
template <typename... Args> decltype(auto) operator()(Args &&... args) && {
  if constexpr (sizeof...(Args) == 0)
    return view_type{std::move(*this)};
  else {
    static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>::value)) // any argument is lazy ?
      return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
    else
      return evaluator_t()(std::move(*this), std::forward<Args>(args)...);
  }
}

// ------------- All the [] operators without lazy arguments -----------------------------

// pass a index_t of the mesh
decltype(auto) operator[](mesh_index_t const &arg) {
  EXPECTS(_mesh.is_within_boundary(arg));
  return dproxy_t::invoke(_data, _mesh.index_to_linear(arg));
}
decltype(auto) operator[](mesh_index_t const &arg) const { return dproxy_t::invoke(_data, _mesh.index_to_linear(arg)); }

// pass a mesh_point of the mesh
decltype(auto) operator[](mesh_point_t const &x) {
#ifdef TRIQS_DEBUG
  if (!mesh_point_compatible_to_mesh(x, _mesh)) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
  return dproxy_t::invoke(_data, x.linear_index());
}

decltype(auto) operator[](mesh_point_t const &x) const {
#ifdef TRIQS_DEBUG
  if (!mesh_point_compatible_to_mesh(x, _mesh)) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
  return dproxy_t::invoke(_data, x.linear_index());
}

// pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait
template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) {
  return dproxy_t::invoke(_data, _mesh.index_to_linear(gf_closest_point<Var, Target>::invoke(this->mesh(), p)));
}
template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) const {
  return dproxy_t::invoke(_data, _mesh.index_to_linear(gf_closest_point<Var, Target>::invoke(this->mesh(), p)));
}

// -------------- operator [] with tuple_com. Distinguich the lazy and non lazy case
public:
template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) & {
  static_assert(sizeof...(U) == get_n_variables<Var>::value, "Incorrect number of argument in [] operator");
  if constexpr ((... or clef::is_any_lazy<U>::value)) // any argument is lazy ?
    return clef::make_expr_subscript(*this, tu);
  else {
    static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
    auto l = [this](auto &&... y) -> decltype(auto) { return details::partial_eval(this, y...); };
    return triqs::tuple::apply(l, tu._t);
  }
}

template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) const & {
  static_assert(sizeof...(U) == get_n_variables<Var>::value, "Incorrect number of argument in [] operator");
  if constexpr ((... or clef::is_any_lazy<U>::value)) // any argument is lazy ?
    return clef::make_expr_subscript(*this, tu);
  else {
    static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
    auto l = [this](auto &&... y) -> decltype(auto) { return details::partial_eval(this, y...); };
    return triqs::tuple::apply(l, tu._t);
  }
}

template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) && {
  static_assert(sizeof...(U) == get_n_variables<Var>::value, "Incorrect number of argument in [] operator");
  if constexpr ((... or clef::is_any_lazy<U>::value)) // any argument is lazy ?
    return clef::make_expr_subscript(std::move(*this), tu);
  else {
    static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
    auto l = [this](auto &&... y) -> decltype(auto) { return details::partial_eval(this, y...); };
    return triqs::tuple::apply(l, tu._t);
  }
}

// ------------- [] with lazy arguments -----------------------------

template <typename Arg> clef::make_expr_subscript_t<this_t const &, Arg> operator[](Arg &&arg) const & {
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

template <typename Arg> clef::make_expr_subscript_t<this_t &, Arg> operator[](Arg &&arg) & {
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

template <typename Arg> clef::make_expr_subscript_t<this_t, Arg> operator[](Arg &&arg) && {
  return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
}

// --------------------- A direct access to the grid point --------------------------

template <typename... Args> decltype(auto) get_from_linear_index(Args &&... args) {
  return dproxy_t::invoke(_data, linear_mesh_index_t(std::forward<Args>(args)...));
}

template <typename... Args> decltype(auto) get_from_linear_index(Args &&... args) const {
  return dproxy_t::invoke(_data, linear_mesh_index_t(std::forward<Args>(args)...));
}

template <typename... Args> decltype(auto) on_mesh(Args &&... args) {
  return dproxy_t::invoke(_data, _mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
}

template <typename... Args> decltype(auto) on_mesh(Args &&... args) const {
  return dproxy_t::invoke(_data, _mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
}

//----------------------------- HDF5 -----------------------------

/// HDF5 name
static std::string hdf5_scheme() { return "Gf"; }

friend struct gf_h5_rw<Var, Target>;

/// Write into HDF5
friend void h5_write(h5::group fg, std::string const &subgroup_name, this_t const &g) {
  auto gr = fg.create_group(subgroup_name);
  gr.write_hdf5_scheme(g);
  gf_h5_rw<Var, Target>::write(gr, g);
}

/// Read from HDF5
friend void h5_read(h5::group fg, std::string const &subgroup_name, this_t &g) {
  auto gr       = fg.open_group(subgroup_name);
  auto tag_file = gr.read_hdf5_scheme();
  if (!(tag_file[0] == 'G' and tag_file[1] == 'f'))
    TRIQS_RUNTIME_ERROR << "h5_read : For a Green function, the type tag should be Gf (or Gfxxxx for old archive) "
                        << " while I found " << tag_file;
  gf_h5_rw<Var, Target>::read(gr, g);
}

//-----------------------------  BOOST Serialization -----------------------------
private:
friend class boost::serialization::access;

public:
/// The serialization as required by Boost
template <class Archive> void serialize(Archive &ar, const unsigned int version) {
  ar &_data;
  ar &_mesh;
  ar &_indices;
}

//----------------------------- print  -----------------------------

/// IO
friend std::ostream &operator<<(std::ostream &out, this_t const &x) { return out << "this_t"; }

//----------------------------- MPI  -----------------------------

// mako <%def name="mpidoc(OP)">
/**
     * Initiate (lazy) MPI ${OP}
     *
     * When the returned object is used at the RHS of operator = or in a constructor of a gf,
     * the MPI ${OP} operation is performed.
     *
     * @group MPI
     * @param g The Green function
     * @param c The MPI communicator (default is world)
     * @param root The root of the broadcast communication in the MPI sense.
     * @return Returns a lazy object describing the object and the MPI operation to be performed.
     *
     */
// mako </%def>

// mako ${mpidoc("Bcast")}
friend void mpi_broadcast(this_t &g, mpi::communicator c = {}, int root = 0) {
  // Shall we bcast mesh ?
  mpi::broadcast(g.data(), c, root);
}

// mako ${mpidoc("Reduce")}
friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_reduce(this_t const &a, mpi::communicator c = {}, int root = 0, bool all = false,
                                                              MPI_Op op = MPI_SUM) {
  return {a(), c, root, all, op};
}

// mako ${mpidoc("Scatter")}
friend mpi_lazy<mpi::tag::scatter, const_view_type> mpi_scatter(this_t const &a, mpi::communicator c = {}, int root = 0) {
  return {a(), c, root, true};
}

// mako ${mpidoc("Gather")}
friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_gather(this_t const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
  return {a(), c, root, all};
}

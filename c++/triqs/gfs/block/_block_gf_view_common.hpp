// ------------- Accessors -----------------------------

/// Direct access to the data array
data_t &data() { return _glist; }

/// Const version
data_t const &data() const { return _glist; }

///
block_names_t const &block_names() const { return _block_names; }

///
std::vector<int> block_sizes() const {
  static_assert(g_t::target_t::rank == 2);
  auto res = std::vector<int>(_glist.size());
  for(auto [i, g]: itertools::enumerate(_glist)){
    EXPECTS(g.target_shape()[0] == g.target_shape()[1]);
    res[i] = g.target_shape()[0];
  }
  return res;
}

int size1() const requires(Arity == 2) { return _glist.size(); }
int size2() const requires(Arity == 2) { return _glist[0].size(); } // FIXME PROTECT
int size() const {
  if constexpr (Arity == 1) {
    return _glist.size();
  } else {
    return size1() * size2();
  }
}

private:
template <typename RHS> void _assign_impl(RHS &&rhs) {

  if constexpr (Arity == 1) {
    for (int w = 0; w < size(); ++w) _glist[w] = rhs[w];
  } else {
    for (int w = 0; w < size1(); ++w)
      for (int v = 0; v < size2(); ++v) _glist[w][v] = rhs(w, v);
  }
  _block_names = rhs.block_names();
}

public:
// ------------- All the call operators without lazy arguments -----------------------------

// First, a simple () returns a view, like for an array...
/// Makes a const view of *this
const_view_type operator()() const { return *this; }
/// Makes a view of *this if it is non const
view_type operator()() { return *this; }

decltype(auto) operator()(int n) const requires(Arity == 1) { return _glist[n]; }
decltype(auto) operator()(int n1, int n2) const requires(Arity == 2) { return _glist[n1][n2]; }
decltype(auto) operator()(int n1, int n2) requires(Arity == 2) { return _glist[n1][n2]; }

// ------------- Call with lazy arguments -----------------------------

// Calls with at least one lazy argument : we make a clef expression, cf clef documentation
template <typename... Args> auto operator()(Args &&... args) & requires(nda::clef::is_clef_expression<Args...>) {
  return clef::make_expr_call(*this, std::forward<Args>(args)...);
}

template <typename... Args> auto operator()(Args &&... args) const &requires(nda::clef::is_clef_expression<Args...>) {
  return clef::make_expr_call(*this, std::forward<Args>(args)...);
}

template <typename... Args> auto operator()(Args &&... args) && requires(nda::clef::is_clef_expression<Args...>) {
  return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
}
// ------------- All the [] operators without lazy arguments -----------------------------

decltype(auto) operator[](int n) const requires(Arity == 1) { return _glist[n]; }
decltype(auto) operator[](int n) requires(Arity == 1) { return _glist[n]; }

// ------------- [] with lazy arguments -----------------------------

template <typename Arg> auto operator[](Arg &&arg) const &requires(nda::clef::is_clef_expression<Arg>) {
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

template <typename Arg> auto operator[](Arg &&arg) & requires(nda::clef::is_clef_expression<Arg>) {
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

template <typename Arg> auto operator[](Arg &&arg) && requires(nda::clef::is_clef_expression<Arg>) {
  return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
}

//----------------------------- HDF5 -----------------------------

/// HDF5 name
static std::string hdf5_format() {
  if constexpr (Arity == 1)
    return "BlockGf";
  else
    return "Block2Gf";
}

/// Write into HDF5
friend void h5_write(h5::group fg, std::string const &subgroup_name, this_t const &g) {
  auto gr = fg.create_group(subgroup_name);
  write_hdf5_format(gr, g);

  if constexpr (Arity == 1) {
    h5_write(gr, "block_names", g.block_names());
    for (int i = 0; i < g.size(); ++i) h5_write(gr, g.block_names()[i], g.data()[i]);
  } else {
    h5_write(gr, "block_names1", g.block_names()[0]);
    h5_write(gr, "block_names2", g.block_names()[1]);
    for (int i = 0; i < g.size1(); ++i)
      for (int j = 0; j < g.size2(); ++j) h5_write(gr, g.block_names()[0][i] + "_" + g.block_names()[1][j], g._glist[i][j]);
  }
}

/// Read from HDF5
friend void h5_read(h5::group fg, std::string const &subgroup_name, this_t &g) {
  auto gr = fg.open_group(subgroup_name);
  // Check the attribute or throw
  auto tag_file     = read_hdf5_format(gr);
  auto tag_expected = this_t::hdf5_format();
  if (tag_file != tag_expected)
    TRIQS_RUNTIME_ERROR << "h5_read : mismatch of the Format tag in the h5 group : found " << tag_file << " while I expected " << tag_expected;
  if constexpr (Arity == 1) {

    auto block_names = h5::h5_read<std::vector<std::string>>(gr, "block_names");
    int s            = block_names.size();
    g._glist.resize(s);
    g._block_names = block_names;
    for (int i = 0; i < s; ++i) h5_read(gr, block_names[i], g._glist[i]);
  } else {
    auto block_names1 = h5::h5_read<std::vector<std::string>>(gr, "block_names1");
    auto block_names2 = h5::h5_read<std::vector<std::string>>(gr, "block_names2");
    auto block_names  = std::vector<std::vector<std::string>>{block_names1, block_names2};
    int s0            = block_names[0].size();
    int s1            = block_names[1].size();
    g._glist.resize(s0);
    g._block_names = block_names;
    for (int i = 0; i < s0; ++i) {
      g._glist[i].resize(s1);
      for (int j = 0; j < s1; ++j) h5_read(gr, block_names[0][i] + "_" + block_names[1][j], g._glist[i][j]);
    }
  }
}

// -------------------------------  iterator  --------------------------------------------------

template <bool is_const> class iterator_impl {
  std::conditional_t<is_const, const this_t *, this_t *> bgf = NULL;
  int n;

  public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = g_t;
  using difference_type   = std::ptrdiff_t;
  using pointer           = std::conditional_t<is_const, const g_t *, g_t *>;
  using reference         = std::conditional_t<is_const, g_t const &, g_t &>;
  using block_gf_ref      = std::conditional_t<is_const, this_t const &, this_t &>;

  iterator_impl() = default;
  iterator_impl(block_gf_ref _bgf, bool at_end = false) : bgf(&_bgf), n(at_end ? bgf->size() : 0) {}
  iterator_impl(block_gf_ref _bgf, int _n) : bgf(&_bgf), n(_n) {}

  operator iterator_impl<true>() const { return iterator_impl<true>(*bgf, n); }

  reference operator*() {
    if constexpr (Arity == 1) {
      return (*bgf)[n];
    } else {
      return (*bgf)(n / bgf->size2(), n % bgf->size2());
    }
  }

  reference operator*() const {
    if constexpr (Arity == 1) {
      return (*bgf)[n];
    } else {
      return (*bgf)(n / bgf->size2(), n % bgf->size2());
    }
  }

  reference operator->() { return operator*(); }

  iterator_impl &operator++() {
    ++n;
    return *this;
  }

  iterator_impl operator++(int) {
    auto it = *this;
    ++n;
    return it;
  }

  bool operator==(iterator_impl const &other) const { return ((bgf == other.bgf) && (n == other.n)); }
  bool operator!=(iterator_impl const &other) const { return (!operator==(other)); }
};

using iterator       = iterator_impl<false>;
using const_iterator = iterator_impl<true>;

//------------

iterator begin() { return {*this, false}; }
const_iterator begin() const { return {*this, false}; }
iterator end() { return {*this, true}; }
const_iterator end() const { return {*this, true}; }
auto cbegin() { return const_view_type(*this).begin(); }
auto cend() { return const_view_type(*this).end(); }

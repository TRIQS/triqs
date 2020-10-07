#pragma once

namespace triqs::gfs {

  /**
    * Initiate (lazy) MPI Bcast
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Bcast operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    *
    */
  // mako ${mpidoc("Bcast")}
  template <typename V, typename T, int Arity> void mpi_broadcast(block_gf<V, T, Arity> &g, mpi::communicator c = {}, int root = 0) {
    // Shall we bcast mesh ?
    mpi::broadcast(g.data(), c, root);
  }

  /**
    * Initiate (lazy) MPI Bcast
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Bcast operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    *
    */
  // mako ${mpidoc("Bcast")}
  template <typename V, typename T, int Arity, bool IsConst>
  void mpi_broadcast(block_gf_view<V, T, Arity, IsConst> &g, mpi::communicator c = {}, int root = 0) {
    // Shall we bcast mesh ?
    mpi::broadcast(g.data(), c, root);
  }

  /**
    * Initiate (lazy) MPI Reduce
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Reduce operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    */
  template <typename V, typename T, int Arity>
  mpi_lazy<mpi::tag::reduce, block_gf_const_view<V, T, Arity>> mpi_reduce(block_gf<V, T, Arity> const &a, mpi::communicator c = {}, int root = 0,
                                                                   bool all = false, MPI_Op op = MPI_SUM) {
    return {a(), c, root, all, op};
  }

  /**
    * Initiate (lazy) MPI Reduce
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Reduce operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    */
  template <typename V, typename T, int Arity, bool IsConst>
  mpi_lazy<mpi::tag::reduce, block_gf_const_view<V, T, Arity>> mpi_reduce(block_gf_view<V, T, Arity, IsConst> const &a, mpi::communicator c = {},
                                                                    int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    return {block_gf_view<V, T, Arity>{a}, c, root, all, op};
  }

} // namespace triqs::gfs

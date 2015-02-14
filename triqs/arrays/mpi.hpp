/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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
#include "../mpi/base.hpp"

namespace triqs {
namespace arrays {

 //--------------------------------------------------------------------------------------------------------
 // The lazy ref made by scatter and co. 
 // Differs from the generic one in that it can make a domain of the (target) array
 template <typename Tag, typename A> struct mpi_lazy_array {
  A const &ref;
  mpi::communicator c;
  int root;
  bool all;

  using domain_type = typename A::domain_type;

  /// compute the array domain of the target array
  domain_type domain() const {
   auto dims = ref.shape();
   long slow_size = first_dim(ref);
 
   if (std::is_same<Tag, mpi::tag::scatter>::value) {
    mpi::mpi_broadcast(slow_size, c, root);
    dims[0] = mpi::slice_length(slow_size - 1, c.size(), c.rank());
   }

   if (std::is_same<Tag, mpi::tag::gather>::value) {
    if (!all) {
     dims[0] = mpi::mpi_reduce(slow_size, c, root); // valid only on root
     if (c.rank() != root) dims[0] = 1;        // valid only on root
    } 
    else
     dims[0] = mpi::mpi_all_reduce(slow_size, c, root); // in this case, it is valid on all nodes
   }
   // mpi::tag::reduce :do nothing

   return domain_type{dims};
  }
 };

 //--------------------------------------------------------------------------------------------------------

#define REQUIRES_IS_ARRAY std14::enable_if_t<is_array_or_view<A>::value>
#define REQUIRES_IS_ARRAY2(TAG) std14::enable_if_t<is_array_or_view<A>::value, mpi_lazy_array<mpi::tag::TAG, A>>

  template <typename A> REQUIRES_IS_ARRAY mpi_broadcast(A &a, mpi::communicator c = {}, int root = 0) {
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_broadcast";
   auto sh = a.shape();
   MPI_Bcast(&sh[0], sh.size(), mpi::mpi_datatype<typename decltype(sh)::value_type>(), root, c.get());
   if (c.rank() != root) resize_or_check_if_view(a, sh);
   MPI_Bcast(a.data_start(), a.domain().number_of_elements(), mpi::mpi_datatype<typename A::value_type>(), root, c.get());
  }

  template <typename A> REQUIRES_IS_ARRAY2(reduce) mpi_reduce (A &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_reduce";
   return {a, c, root, all};
  }

  template <typename A> REQUIRES_IS_ARRAY2(scatter) mpi_scatter (A &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_scatter";
   return {a, c, root, all};
  }

  template <typename A> REQUIRES_IS_ARRAY2(gather) mpi_gather (A &a, mpi::communicator c = {}, int root = 0, bool all = false) {
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_gather";
   return {a, c, root, all};
  }

#undef REQUIRES_IS_ARRAY  
#undef REQUIRES_IS_ARRAY2 

//------------------------------- Delegation of the assign operator of the array class -------------

 // mpi_lazy_array model ImmutableCuboidArray
 template <typename Tag, typename A> struct ImmutableCuboidArray<mpi_lazy_array<Tag, A>> : ImmutableCuboidArray<A> {};

 namespace assignment {

  template <typename LHS, typename Tag, typename A> struct is_special<LHS, mpi_lazy_array<Tag, A>> : std::true_type {};

  // assignment delegation
  template <typename LHS, typename A> struct impl<LHS, mpi_lazy_array<mpi::tag::reduce, A>, 'E', void> {

   using laz_t = mpi_lazy_array<mpi::tag::reduce, A>;
   LHS &lhs;
   laz_t laz;
   impl(LHS &lhs_, laz_t laz_) : lhs(lhs_), laz(laz_) {}

   void invoke() {

    auto rhs_n_elem = laz.ref.domain().number_of_elements();
    void *lhs_p = lhs.data_start();
    void *rhs_p = (void *)laz.ref.data_start();
    auto c = laz.c;
    auto root = laz.root;
    auto D = mpi::mpi_datatype<typename A::value_type>();

    bool in_place = (lhs_p == rhs_p); // to be refined. Overlapping condition
    // some checks.
    if (in_place) {
     if (rhs_n_elem != lhs.domain().number_of_elements())
      TRIQS_RUNTIME_ERROR << "mpi reduce of array : same pointer to data start, but differnet number of elements !";
    } else { // check no overlap
     if (std::abs(lhs.data_start() - laz.ref.data_start()) < rhs_n_elem)
      TRIQS_RUNTIME_ERROR << "mpi reduce of array : overlapping arrays !";
    }

    if (!laz.all) {
     if (in_place)
      MPI_Reduce((c.rank() == root ? MPI_IN_PLACE : rhs_p), rhs_p, rhs_n_elem, D, MPI_SUM, root, c.get());
     else {
      if (c.rank() == root) lhs.resize(laz.domain());
      MPI_Reduce(rhs_p, lhs_p, rhs_n_elem, D, MPI_SUM, root, c.get());
     }
    } else { // all reduce
     if (in_place)
      MPI_Allreduce(MPI_IN_PLACE, rhs_p, rhs_n_elem, D, MPI_SUM, c.get());
     else {
      lhs.resize(laz.domain());
      MPI_Allreduce(rhs_p, lhs_p, rhs_n_elem, D, MPI_SUM, c.get());
     }
    }
   }
};
   // assignment delegation
  template <typename LHS, typename A> struct impl<LHS, mpi_lazy_array<mpi::tag::scatter, A>, 'E', void> {

   using laz_t = mpi_lazy_array<mpi::tag::scatter, A>;
   LHS &lhs;
   laz_t laz;

   impl(LHS &lhs_, laz_t laz_) : lhs(lhs_), laz(laz_) {}

   void invoke() {

    lhs.resize(laz.domain());
    auto c = laz.c;
    auto slow_size = first_dim(laz.ref);
    auto slow_stride = laz.ref.indexmap().strides()[0];
    auto sendcounts = std::vector<int>(c.size());
    auto displs = std::vector<int>(c.size() + 1, 0);
    int recvcount = mpi::slice_length(slow_size - 1, c.size(), c.rank()) * slow_stride;
    auto D = mpi::mpi_datatype<typename A::value_type>();

    for (int r = 0; r < c.size(); ++r) {
     sendcounts[r] = mpi::slice_length(slow_size - 1, c.size(), r) * slow_stride;
     displs[r + 1] = sendcounts[r] + displs[r];
    }

    MPI_Scatterv((void *)laz.ref.data_start(), &sendcounts[0], &displs[0], D, (void *)lhs.data_start(), recvcount, D,
                 laz.root, c.get());
   }
};

   // assignment delegation
 template <typename LHS, typename A> struct impl<LHS, mpi_lazy_array<mpi::tag::gather, A>, 'E', void> {

   using laz_t = mpi_lazy_array<mpi::tag::gather, A>;
   LHS &lhs;
   laz_t laz;

   impl(LHS &lhs_, laz_t laz_) : lhs(lhs_), laz(laz_) {}

   void invoke() { 
    auto c = laz.c;
    auto recvcounts = std::vector<int>(c.size());
    auto displs = std::vector<int>(c.size() + 1, 0);
    int sendcount = laz.ref.domain().number_of_elements();
    void *lhs_p = lhs.data_start();
    const void *rhs_p = laz.ref.data_start();
    auto D = mpi::mpi_datatype<typename A::value_type>();

    auto d= laz.domain();
    if (laz.all || (laz.c.rank() == laz.root)) lhs.resize(d);

    auto mpi_ty = mpi::mpi_datatype<int>();
    if (!laz.all)
     MPI_Gather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, laz.root, c.get());
    else
     MPI_Allgather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, c.get());

    for (int r = 0; r < c.size(); ++r) displs[r + 1] = recvcounts[r] + displs[r];

    if (!laz.all)
     MPI_Gatherv((void *)rhs_p, sendcount, D, lhs_p, &recvcounts[0], &displs[0], D, laz.root, c.get());
    else
     MPI_Allgatherv((void *)rhs_p, sendcount, D, lhs_p, &recvcounts[0], &displs[0], D, c.get());
   }

  };
 }
} //namespace arrays
} // namespace triqs

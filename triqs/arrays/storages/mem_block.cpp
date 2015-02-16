#include <triqs/utility/first_include.hpp>
#include "./mem_block.hpp"
#include <exception>

namespace triqs { namespace arrays { namespace storages { 
#ifdef TRIQS_ARRAYS_DEBUG_COUNT_MEMORY

  __global_memory_allocated_from_c_t __global_memory_allocated_from_c;

  void term_handler() {
   std::cerr << "[triqs:mem_block] End of program : memory still allocated for triqs::arrays : "
             << __global_memory_allocated_from_c.count << std::endl <<"   Python initialized : "<< Py_IsInitialized() << std::endl;
  }

  __global_memory_allocated_from_c_t::__global_memory_allocated_from_c_t() {
   std::set_terminate(*term_handler);
  }

  __global_memory_allocated_from_c_t::~__global_memory_allocated_from_c_t() {
   term_handler();
   if (count != 0) {
    std::cerr << " !=0 : this is an internal error. Terminating the code";
    std::abort();
   }
  }
#endif
 }
}
}

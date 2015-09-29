# Doxygen sources
SET(TRIQS_GFS_SRC_DIR ${TRIQS_SOURCE_DIR}/triqs/gfs)
set_property(GLOBAL APPEND PROPERTY DOXYGEN_SOURCES 
 ${TRIQS_GFS_SRC_DIR}/meshes/linear.hpp 
 ${TRIQS_GFS_SRC_DIR}/meshes/matsubara_freq.hpp 
 ${TRIQS_GFS_SRC_DIR}/meshes/matsubara_time.hpp 
 ${TRIQS_GFS_SRC_DIR}/meshes/product.hpp 
 ${TRIQS_GFS_SRC_DIR}/refreq.hpp 
 ${TRIQS_GFS_SRC_DIR}/domains/matsubara.hpp
 )

all_examples()


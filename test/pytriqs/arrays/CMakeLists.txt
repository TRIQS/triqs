include_directories(${CMAKE_CURRENT_SOURCE_DIR})

cython_module( array_cython array_cython )

add_triqs_test_txt( array_cython )

cython_module( expr_cython expr )

cython_module( bug bug )


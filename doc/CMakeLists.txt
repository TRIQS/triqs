
# Should we use MathJax....
find_package(MathJax)
if (MATHJAX_PATH) 
 message(STATUS "Detected MathJax at ${MATHJAX_PATH}")
 option(Sphinx_Math_Generator_MathJax " " ON)
else()
 message(STATUS "MathJax not found automatically")
 option(Sphinx_Math_Generator_MathJax " " OFF)
 set(MATHJAX_PATH "MathJax" CACHE STRING "Path to the MathJax directory") 
endif()

if (Sphinx_Math_Generator_MathJax)
 set(SPHINX_MATH_GENERATOR "mathjax")
 message(STATUS "Using MathJax")
else (Sphinx_Math_Generator_MathJax)
 set(SPHINX_MATH_GENERATOR "pngmath")
 message(STATUS "Not using MathJax")
endif (Sphinx_Math_Generator_MathJax)

# the local extensions....
file(COPY ../foreignlibs/breathe ../foreignlibs/doxylink ../foreignlibs/mpl ../foreignlibs/autocompile DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# The compiler flags for autocompile
set(TRIQS_LIBRARY_ALL ${TRIQS_LIBRARY_BOOST} ${TRIQS_LIBRARY_PYTHON} ${TRIQS_LIBRARY_MPI} ${TRIQS_LIBRARY_HDF5} ${TRIQS_LIBRARY_LAPACK} ${TRIQS_LIBRARY_FFTW} ${TRIQS_LIBRARY_GMP} ${TRIQS_LIBRARY_GSL} )
set(TRIQS_INCLUDE_ALL ${TRIQS_INCLUDE_BOOST} ${TRIQS_INCLUDE_PYTHON} ${TRIQS_INCLUDE_MPI} ${TRIQS_INCLUDE_HDF5} ${TRIQS_INCLUDE_LAPACK} ${TRIQS_INCLUDE_FFTW} ${TRIQS_INCLUDE_GMP} ${TRIQS_INCLUDE_GSL} )

set( link_libs " ${TRIQS_LIBRARY_ALL}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES link_libs)
endif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
string(REPLACE "boost_for_triqs" " " TMP "${link_libs}")
string(REPLACE "debug" " " TMP "${TMP}")
string(REPLACE "optimized" " " TMP "${TMP}")

if(BOOST_SOURCE_DIR)
 set(  link_boost_for_triqs "-lboost_for_triqs")
 EXECUTE_PROCESS(COMMAND sh -c "ln -sf ${CMAKE_BINARY_DIR}/foreignlibs/boost/libboost_for_triqs.so ${CMAKE_CURRENT_BINARY_DIR}")
else(BOOST_SOURCE_DIR)
 set(  link_boost_for_triqs " ")
endif(BOOST_SOURCE_DIR)


set(DOC_AUTOCOMPILE_INCLUDE ${BOOST_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/foreignlibs ${TRIQS_INCLUDE_ALL}  )
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 set(DOC_AUTOCOMPILE_LINK "-L ${CMAKE_BINARY_DIR}/foreignlibs/boost ${link_boost_for_triqs} -L ${CMAKE_BINARY_DIR}/triqs -ltriqs  ${TMP} ")
 set(DOC_AUTOCOMPILE_DEFINITIONS -std=c++0x -DBOOST_PP_VARIADICS=1 -stdlib=libc++ -I/System/Library/Frameworks/vecLib.framework/Versions/A/Headers)
else()
 set(DOC_AUTOCOMPILE_LINK "-L ${CMAKE_BINARY_DIR}/foreignlibs/boost ${link_boost_for_triqs} -L ${CMAKE_BINARY_DIR}/triqs -ltriqs  ${TMP} -Wl,-rpath=${CMAKE_CURRENT_BINARY_DIR} ")
 set(DOC_AUTOCOMPILE_DEFINITIONS -std=c++0x -DBOOST_PP_VARIADICS=1 )
endif()
set(DOC_AUTOCOMPILE_LD_LIBRARY_PATH " ${CMAKE_BINARY_DIR}/foreignlibs/boost/:${CMAKE_BINARY_DIR}/triqs ")
# to find the triqs_config.h
set(DOC_AUTOCOMPILE_DEFINITIONS ${DOC_AUTOCOMPILE_DEFINITIONS} -DTRIQS_BUILDING_LIBRARY -I ${CMAKE_BINARY_DIR}/Config )

# debug
#message( " Link libs ${DOC_AUTOCOMPILE_LINK}")

EXECUTE_PROCESS(COMMAND sh -c "ln -sf ${CMAKE_BINARY_DIR}/triqs/libtriqs.so ${CMAKE_CURRENT_BINARY_DIR}")

# Dive and find the doxygen sources
add_subdirectory(reference/c++)

# Build the doxygen 
set(DOXYGEN_HTML_OUTPUT ./html/doxy_triqs)
find_package(Doxygen REQUIRED)

# debug 
get_property(DOXYGEN_SOURCES GLOBAL PROPERTY DOXYGEN_SOURCES)
string(REPLACE ";" "  " DOXYGEN_SOURCES_LIST "${DOXYGEN_SOURCES}")
#message(STATUS "Doxygen sources are ${DOXYGEN_SOURCES}") 

# Prepare the Doxyfile
configure_file(${TRIQS_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Generate the conf.py
FILE(GLOB_RECURSE SOURCES *.rst)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
EXECUTE_PROCESS(COMMAND sh -c "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html")
set(DOC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
include(${TRIQS_SOURCE_DIR}/cmake/BuildSphinx.cmake)

# Set dependencies
GET_PROPERTY(PYTHON_DYNAMIC_MODULES_LIST GLOBAL PROPERTY PYTHON_DYNAMIC_MODULES_LIST)
foreach (mod ${PYTHON_DYNAMIC_MODULES_LIST})
 add_dependencies(docs_sphinx ${mod})
endforeach (mod ${PYTHON_DYNAMIC_MODULES_LIST})
set(doxy_top ${CMAKE_CURRENT_BINARY_DIR}/doxy.log)
add_custom_command (OUTPUT ${doxy_top} DEPENDS ${DOXYGEN_SOURCES} COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile > ${doxy_top} )
add_custom_target(docs_doxy ALL DEPENDS ${doxy_top})
add_dependencies(docs_sphinx docs_doxy) # we must first have doxygen process the sources to link to them
add_dependencies(docs_sphinx triqs boost_for_triqs ) # add also mpi ?

# Install
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs
 FILES_MATCHING 
 PATTERN "*.html" 
 PATTERN "*.png"
 PATTERN "*.js"
 PATTERN "_*"
 PATTERN "*.png" 
 PATTERN "*.gif" 
 PATTERN "*.xsl" 
 PATTERN "*.css"
 PATTERN "*.pdf"
 PATTERN "*.py"
 PATTERN "*.txt"
 PATTERN "*.bib"
 PATTERN ".svn" EXCLUDE 
 PATTERN "CVS" EXCLUDE
 )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/agogo DESTINATION share/triqs/themes)

# PDF documentation
if (Build_PDF_Documentation)
 add_dependencies(docs_sphinx_pdf_cpp triqs boost_for_triqs ) # add also mpi ?
 install(FILES ${sphinx_top_pdf} DESTINATION share/doc/triqs/)
endif()


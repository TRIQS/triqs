
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
file(COPY ../foreignlibs/breathe ../foreignlibs/doxylink ../foreignlibs/mpl ../foreignlibs/autorun ../foreignlibs/triqs_example DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#EXECUTE_PROCESS(COMMAND sh -c "ln -sf ${CMAKE_BINARY_DIR}/triqs/libtriqs.so ${CMAKE_CURRENT_BINARY_DIR}")

# Generate the conf.py
FILE(GLOB_RECURSE SOURCES *.rst)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
EXECUTE_PROCESS(COMMAND sh -c "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html")
set(DOC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
include(${TRIQS_SOURCE_DIR}/cmake/BuildSphinx.cmake)

# for the C++ compilation of tests
include_directories( ${CMAKE_SOURCE_DIR} )

#set( link_libs " ${TRIQS_LIBRARY_ALL}")
SET( link_libs ${LAPACK_LIBS}  ${BOOST_LIBRARY} )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES link_libs)
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
link_libraries( ${link_libs} triqs)

macro(all_tests )
FILE(GLOB ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
FOREACH(example ${ExampleList}) 
  STRING(REPLACE ".cpp" "" example_short ${example})
  add_executable( doc_${example_short}  ${CMAKE_CURRENT_SOURCE_DIR}/${example})
  add_dependencies(docs_sphinx doc_${example_short}) ## so that all code examples are compiled before the doc
  add_test_C_simple( doc_${example_short} )
  #add_test ( ${example_short} ${example_short})
ENDFOREACH()
endmacro()

# Dive and find the doxygen sources
add_subdirectory(reference/c++)
add_subdirectory(tutorials/c++)

# Build the doxygen 
set(DOXYGEN_HTML_OUTPUT ./html/doxy_triqs)
find_package(Doxygen REQUIRED)
get_property(DOXYGEN_SOURCES GLOBAL PROPERTY DOXYGEN_SOURCES)
string(REPLACE ";" "  " DOXYGEN_SOURCES_LIST "${DOXYGEN_SOURCES}")
#message(STATUS "Doxygen sources are ${DOXYGEN_SOURCES}") 

# Prepare the Doxyfile
configure_file(${TRIQS_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Set dependencies
GET_PROPERTY(PYTHON_DYNAMIC_MODULES_LIST GLOBAL PROPERTY PYTHON_DYNAMIC_MODULES_LIST)
foreach (mod ${PYTHON_DYNAMIC_MODULES_LIST})
 add_dependencies(docs_sphinx ${mod})
endforeach (mod ${PYTHON_DYNAMIC_MODULES_LIST})
set(doxy_top ${CMAKE_CURRENT_BINARY_DIR}/doxy.log)
add_custom_command (OUTPUT ${doxy_top} DEPENDS ${DOXYGEN_SOURCES} COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile > ${doxy_top} )
add_custom_target(docs_doxy ALL DEPENDS ${doxy_top})
add_dependencies(docs_sphinx docs_doxy) # we must first have doxygen process the sources to link to them
add_dependencies(docs_sphinx triqs)

# Install
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/triqs
 FILES_MATCHING 
 PATTERN "*.html" 
 PATTERN "*.png"
 PATTERN "*.jpg"
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
 PATTERN "*.inv"
 PATTERN ".svn" EXCLUDE 
 PATTERN "CVS" EXCLUDE
 )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/agogo DESTINATION share/triqs/themes)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/themes/triqs DESTINATION share/triqs/themes)

# PDF documentation
if (Build_PDF_Documentation)
 add_dependencies(docs_sphinx_pdf triqs )
 install(FILES ${sphinx_top_pdf} DESTINATION share/doc/triqs/)
endif()


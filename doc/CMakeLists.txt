
# MathJax
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

# the local Sphinx extensions ...
file(COPY sphinxext DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(DIRECTORY sphinxext DESTINATION "share/triqs")

# Generate the conf.py
FILE(GLOB_RECURSE SOURCES *.rst)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
EXECUTE_PROCESS(COMMAND sh -c "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/html")
set(DOC_SOURCE ${CMAKE_CURRENT_BINARY_DIR})

# Get sphinx
find_package(Sphinx)
SET(sphinx_top ${CMAKE_CURRENT_BINARY_DIR}/html/contents.html)
add_custom_command(OUTPUT ${sphinx_top} DEPENDS ${SOURCES}
                   COMMAND  export PYTHONPATH=$$PYTHONPATH:${CMAKE_BINARY_DIR}/tools && ${SPHINXBUILD_EXECUTABLE} -c . -b html ${DOC_SOURCE} html)
add_custom_target(docs_sphinx${DOC_EXT} ALL DEPENDS ${sphinx_top} ${DOC_SOURCE})

# for the C++ compilation of tests
include_directories( ${CMAKE_SOURCE_DIR} )

#set( link_libs " ${TRIQS_LIBRARY_ALL}")
SET( link_libs ${LAPACK_LIBS} )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
 list (REMOVE_DUPLICATES link_libs)
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
link_libraries( ${link_libs} triqs)

# doc depends on lib and and on all python modules (because they  may be loaded in examples).
add_dependencies(docs_sphinx triqs)
GET_PROPERTY(PYTHON_DYNAMIC_MODULES_LIST GLOBAL PROPERTY PYTHON_DYNAMIC_MODULES_LIST)
foreach (mod ${PYTHON_DYNAMIC_MODULES_LIST})
 add_dependencies(docs_sphinx ${mod})
endforeach (mod ${PYTHON_DYNAMIC_MODULES_LIST})

# Build C++ examples ...
macro(all_examples)
FILE(GLOB ExampleList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
FOREACH(example ${ExampleList})
  STRING(REPLACE ".cpp" "" example_short ${example})
  add_executable(doc_${example_short} ${CMAKE_CURRENT_SOURCE_DIR}/${example})
  add_dependencies(docs_sphinx doc_${example_short}) ## so that all code examples are compiled before the doc
ENDFOREACH()
endmacro()

add_subdirectory(reference/)
add_subdirectory(tutorials/)
add_subdirectory(tour/)

# Create a temporary copy of the doc that will be completed by the automatically generated one.
FILE(GLOB_RECURSE all_rst_files RELATIVE ${CMAKE_SOURCE_DIR}/doc *.rst _* *.py *.png *.cpp)
SET(rst_copy_tar  ${CMAKE_CURRENT_BINARY_DIR}/rst_copy.tar)
add_custom_command (OUTPUT ${rst_copy_tar} DEPENDS ${all_rst_files}
 COMMAND cd ${CMAKE_SOURCE_DIR}/doc && tar cf ${rst_copy_tar} ${all_rst_files} && cd ${CMAKE_BINARY_DIR}/doc && tar xf ${rst_copy_tar} )
add_custom_target(rst_copy DEPENDS ${rst_copy_tar})
add_dependencies(docs_sphinx rst_copy) # we must first have copied the sources

# use c++2doc to automatically build the documentation of C++
option(DocWithCpp2doc "Use c++2doc to generate C++ documentation of triqs classes and functions (requires libclang)" ON)
if (DocWithCpp2doc)
 FILE(GLOB_RECURSE all_header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ../triqs/*.hpp ../triqs/*.mhpp)
add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp2py.log DEPENDS ${all_header_files} 
                    COMMAND export PYTHONPATH=\$PYTHONPATH:${CMAKE_BINARY_DIR}/tools/cpp2doc/mako && ${PYTHON_INTERPRETER} 
		    ${CMAKE_BINARY_DIR}/tools/c++2doc.py 
		    -N triqs 
		    ${CMAKE_CURRENT_SOURCE_DIR}/reference/doc_root.hpp 
		    --output_directory=${CMAKE_CURRENT_BINARY_DIR}/cpp2doc_generated
		    --includes=${CMAKE_SOURCE_DIR}/ 
		    --includes=${CMAKE_BINARY_DIR}/Config 
		    --includes=${CMAKE_BINARY_DIR}/include
		    --compiler_options="-DTRIQS_BUILDING_LIBRARY" 
		    --mako_location=${CMAKE_SOURCE_DIR}/tools/ 
		    #2>1 > cpp2py.log
		    ) 
add_custom_target(docs_cpp2doc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp2py.log) 
add_dependencies(docs_cpp2doc triqs) #
add_dependencies(docs_sphinx docs_cpp2doc) # we must have installed the sources before building
endif()

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



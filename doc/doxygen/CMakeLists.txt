# ############
# Doxygen
find_package(Doxygen REQUIRED)

# Configure Doxyfile
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

# Configure markdown files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/overview.md.in ${CMAKE_CURRENT_BINARY_DIR}/overview.md @ONLY)

# Fetch doxygen-awesome-css
include(FetchContent)
FetchContent_Declare(doxyawesome
  GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
  GIT_TAG v2.3.2)
FetchContent_MakeAvailable(doxyawesome)
file(COPY ${doxyawesome_SOURCE_DIR}/doxygen-awesome.css DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${doxyawesome_SOURCE_DIR}/doxygen-awesome-sidebar-only.css DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add custom target for Doxygen
add_custom_target(doxygen ALL
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating Doxygen documentation"
  VERBATIM
)

# ############
# Install
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ COMPONENT documentation DESTINATION share/doc/${PROJECT_NAME}/doxygen
  FILES_MATCHING
  REGEX "\\.(html|pdf|png|gif|jpg|svg|ico|js|xsl|css|py|txt|inv|bib|ttf|woff2|eot|sh)$"
  PATTERN "_*"
)
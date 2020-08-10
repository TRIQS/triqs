#  Copyright Olivier Parcollet 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for mathjax.
# It sets up : MATHJAX_PATH
# 

if(NOT DEFINED MATHJAX_PATH)
  set(TRIAL_PATHS
   $ENV{MATHJAX_ROOT}
   ${MATHJAX_ROOT}
   $ENV{HOME}/MathJax
   /usr/share/javascript/mathjax
   /opt/local/share/javascript/mathjax
   )
  find_path(MATHJAX_PATH MathJax.js ${TRIAL_PATHS} DOC "Root Path of MathJax")
endif()

if(MATHJAX_PATH STREQUAL MATHJAX_PATH-NOTFOUND)
  message(STATUS "No local Mathjax found! Falling back to URL")
  set(MATHJAX_PATH "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.7" CACHE STRING "Root Path of MathJax" FORCE)
endif()

mark_as_advanced(MATHJAX_PATH)

find_package_handle_standard_args(MATHJAX DEFAULT_MSG MATHJAX_PATH)

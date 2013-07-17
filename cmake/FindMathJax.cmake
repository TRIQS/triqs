#  Copyright Olivier Parcollet 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# This module looks for mathjax.
# It sets up : MATHJAX_PATH
# 

SET(TRIAL_PATHS
 $ENV{MATHJAX_ROOT}
 ${MATHJAX_ROOT}
 $ENV{HOME}/MathJax
 /usr/share/javascript/mathjax
 /opt/local/share/javascript/mathjax
 )
FIND_PATH(MATHJAX_PATH MathJax.js ${TRIAL_PATHS} DOC "Root Path of MathJax")

mark_as_advanced(MATHJAX_PATH)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MATHJAX DEFAULT_MSG MATHJAX_PATH)


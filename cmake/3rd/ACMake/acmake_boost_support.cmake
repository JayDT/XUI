include(ans.boost_support)

# Include [and/or link] with boost.
# 
# Use **shared** and **multithread** version.
# Runtime library is multithread (debug) dll.
# Decltype will be supported under MSVC10.
# 
# USAGE: acmake_boost_support([<target>] [COMPONENTS component1 component2
#   ...])
# 
# @param target libs will be linked if it is specified
macro(acmake_boost_support)
    boost_support(${ARGN})
endmacro()

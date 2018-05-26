# OVEN_TO_BOOST_FOUND
# OVEN_TO_BOOST_INCLUDE_DIRS
# answeror at 2011-11-13

include(FindPkgMacros)
findpkg_begin(OVEN_TO_BOOST)

find_path(OVEN_TO_BOOST_INCLUDE_DIR NAMES boost/range/as_container.hpp)

findpkg_finish(OVEN_TO_BOOST)

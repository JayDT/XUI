#
# Find the BinUtil-dev includes and library
#

# This module defines
# BFD_INCLUDE_DIR, where to find ace.h
# BFD_LIBRARIES, the libraries to link against
# BFD_FOUND, if false, you cannot build anything that requires ACE

# also defined, but not for general use are
# BFD_LIBRARY, where to find the ACE library.

set( BFD_FOUND 0 )
if ( UNIX )
    FIND_PATH( BFD_INCLUDE_DIR
      NAMES
        bfd.h
      PATHS
        /usr/include
        /usr/local/include
    DOC
      "Specify include-directories that might contain bfd.h here."
    )
    FIND_LIBRARY( BFD_LIBRARIES
      NAMES
        libbfd.so
      PATHS
        /usr/lib
        /usr/local/lib
      DOC "Specify library-locations that might contain the bfd library here."
    )
    
    if ( BFD_LIBRARIES )
      if ( BFD_INCLUDE_DIR )
        set( BFD_FOUND 1 )
      else ( BFD_INCLUDE_DIR )
        message(FATAL_ERROR "Could not find LIBDLheaders! Please install LIBDL libraries and headers")
      endif ( BFD_INCLUDE_DIR )
    endif ( BFD_LIBRARIES )

    mark_as_advanced( BFD_FOUND BFD_LIBRARIES BFD_INCLUDE_DIR )  
    message( ${ASTATUS} "Found BFD library        : ${BFD_LIBRARIES}")
    message( ${ASTATUS} "Found BFD headers        : ${BFD_INCLUDE_DIR}")
else(WIN32)
    SET( BFD_INCLUDE_DIR "")
    SET( BFD_LIBRARIES "")
endif (UNIX)



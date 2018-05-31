# by default, only use the libclang static lib on MSVC
if (NOT DEFINED LIBCLANG_USE_STATIC_LIBRARY)
    if (MSVC)
        set(LIBCLANG_USE_STATIC_LIBRARY YES)
    else ()
        set(LIBCLANG_USE_STATIC_LIBRARY NO)
    endif ()
endif ()

IF (PLATFORM EQUAL 32)
    set(LLVM_SEARCH_PATHS
        "c:/Program Files (x86)/LLVM/"
        ${LLVM_ROOT}
        $ENV{LLVM_ROOT}
        "c:/Program Files/LLVM/"
    )
ELSE(PLATFORM EQUAL 32)
    set(LLVM_SEARCH_PATHS
        "c:/Program Files/LLVM/"
    )
ENDIF(PLATFORM EQUAL 32)

message("--- SEARCH ${LLVM_SEARCH_PATHS}")

set(LIBCLANG_STATIC_LIBRARY_NAME
    "libclang${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

set(LIBCLANG_SHARED_LIBRARY_NAME
    "libclang${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

# include directories
find_path(LLVM_INCLUDE_DIRS
    NAMES "clang-c/Index.h"
    HINTS ${LLVM_SEARCH_PATHS}
    PATH_SUFFIXES "include"
)

if (LIBCLANG_USE_STATIC_LIBRARY)
    # find static library directory
    find_path(LLVM_LIBRARY_DIR
        NAMES ${LIBCLANG_STATIC_LIBRARY_NAME}
        HINTS ${LLVM_SEARCH_PATHS}
        PATH_SUFFIXES "lib" "bin"
    )
endif ()

# shared library directory
find_path(LLVM_BINARY_DIR
    NAMES ${LIBCLANG_SHARED_LIBRARY_NAME}
    HINTS ${LLVM_SEARCH_PATHS}
    PATH_SUFFIXES "bin" "lib"
)

# unable to find everything
if (NOT LLVM_INCLUDE_DIRS OR
    NOT LLVM_BINARY_DIR OR
    (NOT LLVM_LIBRARY_DIR AND LIBCLANG_USE_STATIC_LIBRARY))
    message(SEND_ERROR 
        "Unable to find LLVM installation. "
        "Make sure that \"LLVM_ROOT\" is set with the installation directory in either an environment variable or through the CMake GUI."
    )
endif ()

set(LIBCLANG_SHARED_LIBRARY
    "${LLVM_BINARY_DIR}/${LIBCLANG_SHARED_LIBRARY_NAME}"
)

if (LIBCLANG_USE_STATIC_LIBRARY)
    set(LIBCLANG_LIBRARY
        "${LLVM_LIBRARY_DIR}/${LIBCLANG_STATIC_LIBRARY_NAME}"
    )
else ()
    # we can assume this is a unix system, in which case we link to the shared object file
    set(LIBCLANG_LIBRARY ${LIBCLANG_SHARED_LIBRARY})
endif ()

message(${ASTATUS} "Found LLVM headers       : ${LLVM_INCLUDE_DIRS}")
message(${ASTATUS} "Found LLVM library       : ${LLVM_LIBRARY_DIR}")
message(${ASTATUS} "Found LLVM binary        : ${LIBCLANG_LIBRARY}")
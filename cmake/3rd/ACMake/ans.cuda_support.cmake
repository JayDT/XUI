include(ans.env)
include(ans.parse_arguments)

# CUDA support.
# 
# USAGE: cuda_support([ARCH <arch number 1> <arch number 2> ...])
macro(cuda_support)
    parse_arguments(
        CUDA_SUPPORT
        "ARCHS"
        ""
        ${ARGN}
        )
    find_package(CUDA REQUIRED)
    foreach(ARCH ${CUDA_SUPPORT_ARCHS})
        list(APPEND CUDA_NVCC_FLAGS "-gencode=arch=compute_${ARCH},code=sm_${ARCH}")
        list(APPEND CUDA_NVCC_FLAGS "-gencode=arch=compute_${ARCH},code=compute_${ARCH}")
    endforeach()
endmacro()

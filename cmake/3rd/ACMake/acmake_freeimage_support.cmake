include(acmake_common)
include(acmake_assert)

macro(acmake_freeimage_support TARGET)
    acmake_assert(ACMAKE_SDK_PATH)
    find_package(FreeImage REQUIRED PATHS "${ACMAKE_SDK_PATH}")
    include_directories(${FREEIMAGE_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${FREEIMAGE_LIBRARIES})
endmacro()

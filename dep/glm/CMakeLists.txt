add_library(glm INTERFACE)

target_include_directories(glm
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

MESSAGE (${IntSTATUS} "glm (0.9.9)")

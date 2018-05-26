add_library(boost INTERFACE)

target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})
target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})


MESSAGE (${IntSTATUS} "Boost (extension)")
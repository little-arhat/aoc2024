
add_library(Utils STATIC utils.cpp)
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

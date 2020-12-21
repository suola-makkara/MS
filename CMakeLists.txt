cmake_minimum_required (VERSION 3.8)
project ("MS")

set (CMAKE_CXX_STANDARD 17)

set (BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

add_subdirectory ("SFML")
add_subdirectory ("MS")

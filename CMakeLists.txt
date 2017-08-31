cmake_minimum_required (VERSION 3.1)
project(repost)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

add_subdirectory(repost-common)
add_subdirectory(repost-server)
add_subdirectory(repost-cli)
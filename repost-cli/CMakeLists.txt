include_directories("../repost-common/")
include_directories("../repost-common/generated/cpp")

add_executable(repost-cli
    client.cpp
    repost-cli.cpp)

add_dependencies(repost-cli repost-common)
target_link_libraries(repost-cli repost-common)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.54.0 REQUIRED COMPONENTS system log)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(repost-cli ${Boost_LIBRARIES})

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf 3.4.0 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(repost-cli ${Protobuf_LITE_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(repost-cli ${CMAKE_THREAD_LIBS_INIT})
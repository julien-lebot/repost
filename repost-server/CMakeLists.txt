include_directories("../repost-common/")
include_directories("../repost-common/generated/cpp")

add_executable(repost-server
    channel_manager.cpp
    channel.cpp
    client.cpp
    message_handler.cpp
    repost-server.cpp)

add_dependencies(repost-server repost-common)
target_link_libraries(repost-server repost-common)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.62.0 REQUIRED COMPONENTS system log)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(repost-server ${Boost_LIBRARIES})

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf 3.4.0 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(repost-server ${Protobuf_LITE_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(repost-server ${CMAKE_THREAD_LIBS_INIT})
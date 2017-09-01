include_directories("../repost-common/")
include_directories("../repost-common/generated/cpp")

add_executable(repost-server
    channel_manager.cpp
    message_handler.cpp
    remote_client.cpp
    repost-server.cpp
    server.cpp)

add_dependencies(repost-server repost-common)
target_link_libraries(repost-server repost-common)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.64.0 REQUIRED COMPONENTS system log program_options)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(repost-server ${Boost_LIBRARIES})

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf 3.4.0 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(repost-server libprotobuf-lite.a)

find_package(Threads REQUIRED)
target_link_libraries(repost-server ${CMAKE_THREAD_LIBS_INIT})
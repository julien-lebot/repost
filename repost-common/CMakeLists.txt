include_directories("generated/cpp")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.64.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf 3.4.0 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_library(repost-common STATIC
            client.cpp
            frame.cpp
            generated/cpp/repost-common/repost.pb.cc)

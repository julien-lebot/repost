include_directories("generated/cpp")

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf 3.4.0 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_library(repost-common STATIC
            frame.cpp
            generated/cpp/repost-common/repost.pb.cc)

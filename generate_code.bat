@echo off
protobuf\cmake\build\Release\protoc.exe --cpp_out="repost-common\generated\cpp" --csharp_out="repost-common\generated\cs" --js_out="repost-common\generated\js" repost-common\repost.proto 
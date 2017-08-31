#!/bin/sh
set -e
# check to see if protobuf folder is empty
if [ ! -d "$HOME/protobuf/lib" ]; then
  wget https://github.com/google/protobuf/releases/download/v3.4.0/protobuf-cpp-3.4.0.tar.gz
  tar -xzvf protobuf-cpp-3.4.0.tar.gz
  cd protobuf-3.4.0 && ./configure --prefix=$HOME/protobuf && make && make install
else
  echo "Using cached directory."
fi

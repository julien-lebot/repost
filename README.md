# repost

[![Build Status](https://travis-ci.org/julien-lebot/repost.svg?branch=master)](https://travis-ci.org/julien-lebot/repost)


Simple &amp; straightforward pubsub based on boost::asio & google's protobuf.

# Prerequisites
 * CMake (for protobuf)
 * Visual Studio 2017 (for Windows)

# Compilation on Windows:

Clone the repository

    λ git clone --recursive ...
    
Build protobuf with 

    λ make_protobuf.bat
    
Generate the protocol code with

    λ generate_code.bat`
    
Open the Visual Studio solution & compile

# Compilation on Linux:

(Only tested with Ubuntu 17.04)
Install the prerequisites
    - boost 1.62+
    - cmake
    
Clone the repository
 
    λ git clone --recursive ...
 
Retrieve the correct version of protobuf (3.4.0)
 
    λ cd protobuf
    λ git checkout tags/v3.4.0

Follow the compilation instruction for protobuf

    λ ./configure
    λ make
    λ make check
    λ sudo make install
    λ sudo ldconfig
    
Build the project

    λ cd ..
    λ mkdir build
    λ cd build
    λ cmake ..
    λ make

# Usage

## repost-server
`Usage: repost-server <port>`

## repost-cli
`Usage: repost-cli <host> <port>`

### Commands:
* `SUBSCRIBE <channel> [<channel> ...]`

  Subscribes to one or more channels separated by space.
  
* `UNSUBSCRIBE <channel> [<channel> ...]`

  Unsubscribes to one or more channels separated by space.
  
* `PUBLISH <channel> <payload>`

  Publishes a payload to a channel. The payload may contain space(s).

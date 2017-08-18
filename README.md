# repost
Simple &amp; straightforward pubsub based on boost::asio & google's protobuf.

# Prerequisites
 * CMake (for protobuf)
 * Visual Studio 2017 (for Windows)

# Compilation on Windows:

* Clone the repository
* Build protobuf with 
`make_protobuf.bat`
* Generate the protocol code with
`generate_code.bat`
* Open the Visual Studio solution & compile

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

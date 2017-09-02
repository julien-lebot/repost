#include "stdafx.h"

#include <cstring>
#include <google/protobuf/wire_format_lite.h>

#include "frame.hpp"


namespace repost
{
    frame::frame() : _bodyLength(0)
    {
    }

    frame::frame(const char* data)
    {
        body_length(std::strlen(data));
        std::memcpy(body(), data, body_length());
        encode_header();
    }

    frame::frame(const std::string& data)
    {
        body_length(data.length());
        std::memcpy(body(), data.c_str(), body_length());
        encode_header();
    }

    const char* frame::data() const
    {
        return _data;
    }

    char* frame::data()
    {
        return _data;
    }

    uint32_t frame::length() const
    {
        return header_length + _bodyLength;
    }

    const char* frame::body() const
    {
        return _data + header_length;
    }

    char* frame::body()
    {
        return _data + header_length;
    }

    uint32_t frame::body_length() const
    {
        return _bodyLength;
    }

    void frame::body_length(uint32_t new_length)
    {
        _bodyLength = new_length;
        if (_bodyLength > max_body_length)
        {
            _bodyLength = max_body_length;
        }
    }

    bool frame::decode_header()
    {
        google::protobuf::io::CodedInputStream::ReadLittleEndian32FromArray
        (
            reinterpret_cast<uint8_t*>(_data),
            &_bodyLength
        );

        if (_bodyLength > max_body_length)
        {
            _bodyLength = 0;
            return false;
        }
        return true;
    }

    void frame::encode_header()
    {
        google::protobuf::io::CodedOutputStream::WriteLittleEndian32ToArray
        (
            _bodyLength,
            reinterpret_cast<uint8_t*>(_data)
        );
    }
}
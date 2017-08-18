#include "stdafx.h"

#include <cstdio>
#include <cstring>
#include "Frame.hpp"

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
    return data_;
}

char* frame::data()
{
    return data_;
}

std::size_t frame::length() const
{
    return header_length + _bodyLength;
}

const char* frame::body() const
{
    return data_ + header_length;
}

char* frame::body()
{
    return data_ + header_length;
}

std::size_t frame::body_length() const
{
    return _bodyLength;
}

void frame::body_length(std::size_t new_length)
{
    _bodyLength = new_length;
    if (_bodyLength > max_body_length)
    {
        throw std::exception("new_length too big");
    }
}

bool frame::decode_header()
{
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    _bodyLength = std::atoi(header);
    if (_bodyLength > max_body_length)
    {
        _bodyLength = 0;
        return false;
    }
    return true;
}

void frame::encode_header()
{
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(_bodyLength));
    std::memcpy(data_, header, header_length);
}
#ifndef __FRAME_HPP__
#define __FRAME_HPP__

#include <string>

class frame
{
public:
    enum { header_length = 4 };
    enum { max_body_length = 8192 };

    frame();
    explicit frame(const char *data);
    explicit frame(const std::string &data);

    const char* data() const;

    char* data();

    std::size_t length() const;

    const char* body() const;

    char* body();

    std::size_t body_length() const;

    void body_length(std::size_t new_length);

    bool decode_header();

    void encode_header();

private:
    char data_[header_length + max_body_length];
    std::size_t _bodyLength;
};

#endif

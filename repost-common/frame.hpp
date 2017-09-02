#ifndef __FRAME_HPP__
#define __FRAME_HPP__

#include <string>

namespace repost
{
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

        uint32_t length() const;

        const char* body() const;

        char* body();

        uint32_t body_length() const;

        void body_length(uint32_t new_length);

        bool decode_header();

        void encode_header();

    private:
        char _data[header_length + max_body_length];
        uint32_t _bodyLength;
    };
}
#endif

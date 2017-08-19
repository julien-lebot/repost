#include "stdafx.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tokenizer.hpp>
#include <boost/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <generated/cpp/repost-common/repost.pb.h>

#include "client.hpp"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::info
        );

        if (argc < 3)
        {
            std::cerr << "Usage: repost-cli <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });

        client c(io_service, endpoint_iterator);

        boost::thread t([&io_service]() { io_service.run(); });
        boost::char_separator<char> _sep{ " " };
        char line[frame::max_body_length + 1];
        while (std::cin.getline(line, frame::max_body_length + 1))
        {
            std::string lineStr(line);
            boost::tokenizer<boost::char_separator<char>> tokenizer{ lineStr, _sep };
            std::vector<std::string> tokens;
            for (auto token : tokenizer)
            {
                tokens.push_back(token);
            }
            if (tokens.size() >= 3 && tokens[0] == "PUBLISH")
            {
                auto payloadStart = lineStr.find(tokens[2]);
                c.publish(tokens[1], lineStr.substr(payloadStart));
            }
            else if (tokens.size() >= 2 && tokens[0] == "SUBSCRIBE")
            {
                c.subscribe({ tokens.begin() + 1, tokens.end() });
            }
            else if (tokens.size() >= 2 && tokens[0] == "UNSUBSCRIBE")
            {
                c.unsubscribe({ tokens.begin() + 1, tokens.end() });
            }
        }

        c.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}


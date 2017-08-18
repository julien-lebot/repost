// repost-cli.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <random>
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

        if (argc == 5 && (strcmp(argv[3], "benchmark_publish") == 0 ||
                          strcmp(argv[3], "benchmark_subscribe") == 0))
        {
            std::random_device r;
            std::default_random_engine randomEngine(r());
            std::uniform_int_distribution<int> randomChannel(1, 1024);
            int nThreads = atoi(argv[4]);
            boost::thread_group threadGroup;
            std::vector<client*> clients;
            bool stop = false;
            for (int i = 0; i < nThreads; ++i)
            {
                client* c = new client(io_service, endpoint_iterator);
                clients.push_back(c);
                if (strcmp(argv[3], "benchmark_subscribe") == 0)
                {
                    std::vector<std::string> channels;
                    for (int channelNumber = 0; channelNumber < randomChannel(randomEngine); ++channelNumber)
                    {
                        std::stringstream sstr;
                        sstr << "channel:" << randomChannel(randomEngine);
                        channels.push_back(sstr.str());
                    }
                    c->subscribe(channels);
                }
                else
                {
                    threadGroup.create_thread([=, &randomEngine]()
                    {
                        while (!stop)
                        {
                            std::stringstream sstr;
                            sstr << "channel:" << randomChannel(randomEngine);
                            c->publish(sstr.str(), "LOREM IPSUM DOLORES");
                        }
                    });
                }
            }
            threadGroup.create_thread([&io_service]() { io_service.run(); });
            threadGroup.join_all();
            for (auto client : clients)
            {
                delete client;
            }
            clients.clear();
        }
        else
        {
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

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}


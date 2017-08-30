#include "stdafx.h"

#include <future>
#include <boost/tokenizer.hpp>
#include <boost/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#include <generated/cpp/repost-common/repost.pb.h>

#include "client.hpp"

using boost::asio::ip::tcp;
namespace po = boost::program_options;

static const char* HelpFlag = "help";
static const char* HostFlag = "host";
static const char* PortFlag = "port";
static const char* BenchmarkFlag = "benchmark";
static const char* ChannelsFlag = "channel";

//  Return current system clock as milliseconds
static int64_t s_clock()
{
#if (defined (WIN32))
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    unsigned __int64 largeInt = fileTime.dwHighDateTime;
    largeInt <<= 32;
    largeInt |= fileTime.dwLowDateTime;
    largeInt /= 10000; // FILETIME is in units of 100 nanoseconds
    return static_cast<int64_t>(largeInt);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

void do_benchmark(repost::client &client, po::variables_map const &vm)
{
    uint32_t requests;
    int64_t start;
    start = s_clock();
    uint32_t nRequests = vm[BenchmarkFlag].as<uint32_t>();
    std::vector<std::string> channels = vm[ChannelsFlag].as<std::vector<std::string>>();
    std::cout << "Testing with " << nRequests << " requests…" << std::endl;
    for (requests = 0; requests < nRequests; requests++)
    {
        for (const auto &channel : channels)
        {
            client.publish(channel, "LOREM IPSUM DOLORES");
        }
    }

    std::cout << (1000 * nRequests) / static_cast<int>(s_clock() - start) << " calls/second" << std::endl;
    client.close();
}

int main(int argc, char* argv[])
{
    try
    {
        po::positional_options_description p;
        p.add(ChannelsFlag, -1);

        po::options_description desc("Allowed options");
        desc.add_options()
            (HelpFlag, "produce help message")
            (HostFlag, po::value<std::string>(), "set the host to connect to")
            (PortFlag, po::value<uint16_t>(), "set the port to connect to")
            (BenchmarkFlag, po::value<uint32_t>(), "set the number of test messages to send to a test channel")
            (ChannelsFlag, po::value<std::vector<std::string>>(), "if used with --benchmark, sets the test channels to send the benchmark messages to. Else listens to the given channels.")
            ;

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count(HelpFlag) ||
            !vm.count(HostFlag) ||
            !vm.count(PortFlag))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count(BenchmarkFlag) && !vm.count(ChannelsFlag))
        {
            std::cerr << "Missing channel to post the messages to" << std::endl;
            return 1;
        }

        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::info
        );


        boost::asio::io_service io_service;

        std::cout << "Connecting to " << vm[HostFlag].as<std::string>() << ":" << vm[PortFlag].as<uint16_t>() << "... " << std::endl;
        tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ vm[HostFlag].as<std::string>(), boost::lexical_cast<std::string>(vm[PortFlag].as<uint16_t>()) });

        repost::client client(io_service);
        auto futureResult = client.connect(endpoint_iterator);

        auto status = futureResult.wait_for(boost::chrono::milliseconds(500));
        if (status == boost::future_status::timeout)
        {
            std::cerr << "Timeout" << std::endl;
            client.close();
            return 1;
        }

        boost::thread t([&io_service]() { io_service.run(); });

        auto errorCode = futureResult.get();
        if (errorCode)
        {
            std::cerr << errorCode.message() << std::endl;
            return 1;
        }

        if (vm.count(BenchmarkFlag))
        {
            do_benchmark(client, vm);
            t.join();
            return 0;
        }

        client.on_message_received([](const repost::Publication &publication)
        {
            std::cout << "From: " << publication.channel() << std::endl
                      << "Message: " << publication.payload() << std::endl;
        });

        if (vm.count(ChannelsFlag))
        {
            client.subscribe(vm[ChannelsFlag].as<std::vector<std::string>>());
        }

        boost::char_separator<char> _sep{ " " };
        char line[repost::frame::max_body_length + 1];
        while (std::cin.getline(line, repost::frame::max_body_length + 1))
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
                client.publish(tokens[1], lineStr.substr(payloadStart));
            }
            else if (tokens.size() >= 2 && tokens[0] == "SUBSCRIBE")
            {
                client.subscribe({ tokens.begin() + 1, tokens.end() });
            }
            else if (tokens.size() >= 2 && tokens[0] == "UNSUBSCRIBE")
            {
                client.unsubscribe({ tokens.begin() + 1, tokens.end() });
            }
        }

        client.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}


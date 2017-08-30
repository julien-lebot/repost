#include "stdafx.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

#include "server.hpp"

using boost::asio::ip::tcp;
namespace po = boost::program_options;

static const char* HelpFlag = "help";
static const char* HostFlag = "host";
static const char* PortFlag = "port";

int main(int argc, char* argv[])
{
    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            (HelpFlag, "produce help message")
            (PortFlag, po::value<uint16_t>(), "set the port to connect to")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count(HelpFlag) ||
            !vm.count(PortFlag))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::info
        );

        boost::asio::io_service io_service;

        repost::server server(io_service, tcp::endpoint(tcp::v4(), vm[PortFlag].as<uint16_t>()));
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

// repost-server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <generated/cpp/repost-common/repost.pb.cc>

#include "channel_manager.hpp"
#include "message_handler.hpp"
#include "client.hpp"

using boost::asio::ip::tcp;

class server
{
public:
    server(
        boost::asio::io_service& io_service,
        const tcp::endpoint& endpoint)
    : _acceptor(io_service, endpoint)
    , _socket(io_service)
    , _channelManager()
    , _messageHandler(_channelManager)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        _acceptor.async_accept(_socket,
            [this](boost::system::error_code ec)
        {
            if (!ec)
            {
                BOOST_LOG_TRIVIAL(info) << "New client connected from " << _socket.remote_endpoint().address() << ":" << _socket.remote_endpoint().port();
                std::make_shared<client>(std::move(_socket), _messageHandler)->start();
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Error accepting client: " << ec.message();
            }
            do_accept();
        });
    }

    tcp::acceptor _acceptor;
    tcp::socket _socket;
    channel_manager _channelManager;
    message_handler _messageHandler;
};

int main(int argc, char* argv[])
{
    try
    {
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::info
        );

        if (argc < 2)
        {
            std::cerr << "Usage: repost-server <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        server server(io_service, tcp::endpoint(tcp::v4(), std::atoi(argv[1])));
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}


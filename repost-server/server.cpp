#include "stdafx.h"
#include "server.hpp"
#include "remote_client.hpp"

namespace repost
{
    using boost::asio::ip::tcp;

    server::server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
        : _acceptor(io_service, endpoint)
        , _socket(io_service)
        , _channelManager()
        , _messageHandler(_channelManager)
    {
        do_accept();
    }

    void server::do_accept()
    {
        _acceptor.async_accept(_socket,
            [this](boost::system::error_code ec)
        {
            if (!ec)
            {
                BOOST_LOG_TRIVIAL(info) << "New client connected from " << _socket.remote_endpoint().address() << ":" << _socket.remote_endpoint().port();
                std::make_shared<remote_client>(std::move(_socket), _messageHandler)->start();
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Error accepting client: " << ec.message();
            }
            do_accept();
        });
    }
}

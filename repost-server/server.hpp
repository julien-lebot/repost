#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <boost/asio.hpp>

#include "channel_manager.hpp"
#include "message_handler.hpp"

namespace repost
{
    class server
    {
    public:
        server(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& endpoint);

    private:
        void do_accept();

        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::socket _socket;
        channel_manager _channelManager;
        message_handler _messageHandler;
    };
}

#endif // __SERVER_HPP__
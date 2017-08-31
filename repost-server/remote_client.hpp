#ifndef __REMOTE_CLIENT_HPP__
#define __REMOTE_CLIENT_HPP__

#include <deque>
#include <memory>
#include <boost/asio.hpp>
#include <frame.hpp>
#include "message_handler.hpp"

namespace repost
{
    class message_handler;

    class remote_client
        : public std::enable_shared_from_this<remote_client>
    {
        typedef std::deque<frame> frame_queue;
    public:
        remote_client(boost::asio::ip::tcp::socket socket, message_handler& messageHandler);

        void start();

        void deliver(const std::string& msg);

    private:
        void do_read_header();

        void do_read_body();

        void do_write();

        boost::asio::ip::tcp::socket _socket;
        message_handler _messageHandler;
        frame _incomingFrame;
        // TODO: Optimization, drop messages when queue full or get rid of queue
        frame_queue _outboundMessages;
    };

    typedef std::shared_ptr<remote_client> client_ptr;
}

#endif // __REMOTE_CLIENT_HPP__
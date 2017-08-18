#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <deque>
#include <memory>
#include <boost/asio.hpp>
#include <frame.hpp>
#include "message_handler.hpp"

using boost::asio::ip::tcp;

class message_handler;

class client
    : public std::enable_shared_from_this<client>
{
    typedef std::deque<frame> frame_queue;
public:
    client(tcp::socket socket, message_handler& messageHandler);

    void start();

    void deliver(const std::string& msg);

private:
    void do_read_header();

    void do_read_body();

    void do_write();

    tcp::socket _socket;
    message_handler _messageHandler;
    frame _incomingFrame;
    // TODO: Optimization, drop messages when queue full or get rid of queue
    frame_queue _outboundMessages;
};

typedef std::shared_ptr<client> client_ptr;

#endif // __CLIENT_HPP__
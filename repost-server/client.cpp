#include "stdafx.h"
#include "client.hpp"
#include "Frame.hpp"

client::client(tcp::socket socket, message_handler& messageHandler)
: _socket(std::move(socket))                                   
, _messageHandler(messageHandler)
{
}

void client::start()
{
    do_read_header();
}

void client::deliver(const std::string& msg)
{
    BOOST_LOG_TRIVIAL(trace) << "Delivering new message";
    bool write_in_progress = !_outboundMessages.empty();
    _outboundMessages.push_back(std::move(frame(msg)));
    if (!write_in_progress)
    {
        BOOST_LOG_TRIVIAL(trace) << "No outstanding messages found, writing immediately";
        do_write();
    }
    else
    {
        BOOST_LOG_TRIVIAL(trace) << _outboundMessages.size() << " messages already queued, queuing new message";
    }
}

void client::do_read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_incomingFrame.data(), frame::header_length),
                            [this, self](class boost::system::error_code ec, std::size_t /*length*/)
                    {
                        if (!ec && _incomingFrame.decode_header())
                        {
                            do_read_body();
                        }
                        else
                        {
                            BOOST_LOG_TRIVIAL(error) << "Error reading header: " << ec.message();
                            _messageHandler.disconnect(shared_from_this());
                        }
                    });
}

void client::do_read_body()
{
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_incomingFrame.body(), _incomingFrame.body_length()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                    {
                        if (!ec)
                        {
                            _messageHandler.handle(_incomingFrame, shared_from_this());
                            do_read_header();
                        }
                        else
                        {
                            BOOST_LOG_TRIVIAL(error) << "Error reading body: " << ec.message();
                            _messageHandler.disconnect(shared_from_this());
                        }
                    });
}

void client::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(_socket,
                             boost::asio::buffer(_outboundMessages.front().data(),
                                                 _outboundMessages.front().length()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
                     {
                         if (!ec)
                         {
                             _outboundMessages.pop_front();
                             if (!_outboundMessages.empty())
                             {
                                 BOOST_LOG_TRIVIAL(trace) << _outboundMessages.size() << " outsanding messages found, writing";
                                 do_write();
                             }
                         }
                         else
                         {
                             BOOST_LOG_TRIVIAL(error) << "Error writing message: " << ec.message();
                             _messageHandler.disconnect(shared_from_this());
                         }
                     });
}

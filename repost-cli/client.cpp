#include "stdafx.h"

#include <iostream>
#include <generated/cpp/repost-common/repost.pb.h>

#include "client.hpp"

using boost::asio::ip::tcp;

client::client(
    boost::asio::io_service& io_service,
    tcp::resolver::iterator endpoint_iterator)
    : _ioService(io_service)
    , _socket(io_service)
{
    do_connect(endpoint_iterator);
}

void client::subscribe(std::vector<std::string> const& channels)
{
    repost::Message msg;
    for (const auto &channel : channels)
    {
        msg.mutable_subscribe()->add_channels(channel);
    }
    write(std::move(frame(msg.SerializeAsString())));
}

void client::unsubscribe(std::vector<std::string> const& channels)
{
    repost::Message msg;
    for (const auto &channel : channels)
    {
        msg.mutable_unsubscribe()->add_channels(channel);
    }
    write(std::move(frame(msg.SerializeAsString())));
}

void client::publish(std::string const& channel, std::string const& payload)
{
    repost::Message msg;
    msg.mutable_publish()->set_channel(channel);
    msg.mutable_publish()->set_payload(payload);
    write(std::move(frame(msg.SerializeAsString())));
}

void client::write(const frame& msg)
{
    if (_outboundMessages.write_available() < 1)
    {
        _outboundMessages.pop();
    }
    bool write_in_progress = !_outboundMessages.empty();
    _outboundMessages.push(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}

void client::close()
{
    BOOST_LOG_TRIVIAL(info) << "Closing socket";
    _ioService.post([this]() { _socket.close(); });
}

void client::do_connect(tcp::resolver::iterator endpoint_iterator)
{
    boost::asio::async_connect(_socket, endpoint_iterator,
                               [this](boost::system::error_code ec, tcp::resolver::iterator)
                       {
                           if (!ec)
                           {
                               do_read_header();
                           }
                           else
                           {
                               BOOST_LOG_TRIVIAL(error) << "Error connecting to server: " << ec.message();
                           }
                       });
}

void client::do_read_header()
{
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_incomingMessage.data(), frame::header_length),
                            [this](boost::system::error_code ec, std::size_t /*length*/)
                    {
                        if (!ec && _incomingMessage.decode_header())
                        {
                            do_read_body();
                        }
                        else
                        {
                            BOOST_LOG_TRIVIAL(error) << "Error reading header: " << ec.message();
                            _socket.close();
                        }
                    });
}

void client::do_read_body()
{
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_incomingMessage.body(), _incomingMessage.body_length()),
                            [this](boost::system::error_code ec, std::size_t /*length*/)
                    {
                        if (!ec)
                        {
                            std::cout.write(_incomingMessage.body(), _incomingMessage.body_length());
                            std::cout << "\n";
                            do_read_header();
                        }
                        else
                        {
                            BOOST_LOG_TRIVIAL(error) << "Error reading body: " << ec.message();
                            _socket.close();
                        }
                    });
}

void client::do_write()
{
    _outboundMessages.pop(_outgoingMessage);

    boost::asio::async_write(_socket,
                             boost::asio::buffer(
                                 _outgoingMessage.data(),
                                 _outgoingMessage.length()),
                             [this](boost::system::error_code ec, std::size_t /*length*/)
                     {
                         if (!ec)
                         {
                             if (_outboundMessages.read_available() > 0)
                             {
                                 do_write();
                             }
                         }
                         else
                         {
                             BOOST_LOG_TRIVIAL(error) << "Error writing message: " << ec.message();
                             _socket.close();
                         }
                     });
}

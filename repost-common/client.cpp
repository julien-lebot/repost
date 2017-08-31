#include "stdafx.h"

#include <iostream>
#include <boost/asio/use_future.hpp>
#include <boost/log/trivial.hpp>

#include "generated/cpp/repost-common/repost.pb.h"
#include "client.hpp"

namespace repost
{
    using boost::asio::ip::tcp;

    client::client(
        boost::asio::io_service& io_service)
        : _ioService(io_service)
        , _socket(io_service)
    {
    }

    void client::subscribe(std::vector<std::string> const& channels)
    {
        Message msg;
        for (const auto &channel : channels)
        {
            msg.mutable_subscribe()->add_channels(channel);
        }
        write(std::move(frame(msg.SerializeAsString())));
    }

    void client::unsubscribe(std::vector<std::string> const& channels)
    {
        Message msg;
        for (const auto &channel : channels)
        {
            msg.mutable_unsubscribe()->add_channels(channel);
        }
        write(std::move(frame(msg.SerializeAsString())));
    }

    void client::publish(std::string const& channel, std::string const& payload)
    {
        Message msg;
        msg.mutable_publication()->set_channel(channel);
        msg.mutable_publication()->set_payload(payload);
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
            _ioService.post([this]() { do_write(); });
        }
    }

    void client::close()
    {
        BOOST_LOG_TRIVIAL(info) << "Closing socket";
        _ioService.post([this]() { _socket.close(); });
    }

    boost::signals2::connection client::on_message_received(const publication_signals::slot_type& slot)
    {
        return _messageReceived.connect(slot);
    }

    boost::signals2::connection client::on_connection_changed(const connection_signal::slot_type & slot)
    {
        return _connectionChanged.connect(slot);
    }

    boost::unique_future<boost::system::error_code> client::connect(tcp::resolver::iterator endpoint_iterator)
    {
        BOOST_LOG_TRIVIAL(info) << "Connecting to: " << endpoint_iterator->host_name() << ":" << endpoint_iterator->service_name();
        auto promise = std::make_shared<boost::promise<boost::system::error_code>>();
        boost::asio::async_connect(_socket, endpoint_iterator,
            [this, promise]
        (boost::system::error_code ec, tcp::resolver::iterator iterator)
        {
            if (!ec)
            {
                _connectionChanged(true);
                do_read_header();
            }
            promise->set_value(ec);
        });
        
        return promise->get_future();
    }

    void client::do_read_header()
    {
        boost::asio::async_read(_socket,
            boost::asio::buffer(_incomingMessage.data(), frame::header_length),
            [this](boost::system::error_code ec, size_t /*length*/)
        {
            if (!ec && _incomingMessage.decode_header())
            {
                do_read_body();
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Error reading header: " << ec.message();
                on_disconnected();
            }
        });
    }

    void client::do_read_body()
    {
        boost::asio::async_read(_socket,
            boost::asio::buffer(_incomingMessage.body(), _incomingMessage.body_length()),
            [this](boost::system::error_code ec, size_t /*length*/)
        {
            if (!ec)
            {
                Publication publication;
                publication.ParseFromArray(_incomingMessage.body(), _incomingMessage.body_length());
                _messageReceived(publication);
                do_read_header();
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Error reading body: " << ec.message();
                on_disconnected();
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
            [this](boost::system::error_code ec, size_t /*length*/)
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
                on_disconnected();
            }
        });
    }

    void client::on_disconnected()
    {
        _connectionChanged(false);
        _socket.close();
    }
}

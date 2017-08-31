#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <deque>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/thread/future.hpp>

#include "frame.hpp"

namespace repost
{
    class Publication;
    class client : boost::noncopyable
    {
        enum { high_water_mark = 64 };
        typedef boost::lockfree::spsc_queue<frame, boost::lockfree::capacity<high_water_mark>> frame_queue;
    public:
        typedef boost::signals2::signal<void(const Publication &)> publication_signals;
        typedef boost::signals2::signal<void(bool)> connection_signal;

        client(boost::asio::io_service& io_service);

        void subscribe(std::vector<std::string> const &channels);
        void unsubscribe(std::vector<std::string> const &channels);
        void publish(std::string const &channel, std::string const &payload);

        boost::unique_future<boost::system::error_code> connect(
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
        void close();

        boost::signals2::connection on_message_received(const publication_signals::slot_type &slot);
        boost::signals2::connection on_connection_changed(const connection_signal::slot_type &slot);
    private:

        void write(const frame& msg);

        void do_read_header();

        void do_read_body();

        void do_write();

        void on_disconnected();

        boost::asio::io_service& _ioService;
        boost::asio::ip::tcp::socket _socket;
        frame _incomingMessage;
        frame _outgoingMessage;
        frame_queue _outboundMessages;
        publication_signals _messageReceived;
        connection_signal _connectionChanged;
    };
}

#endif // __CLIENT_HPP__
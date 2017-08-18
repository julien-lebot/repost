#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <deque>
#include <boost/asio.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <frame.hpp>

class client
{
    enum { high_water_mark = 64 };
    typedef boost::lockfree::spsc_queue<frame, boost::lockfree::capacity<high_water_mark>> frame_queue;
public:
    client(boost::asio::io_service& io_service,
           boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void subscribe(std::vector<std::string> const &channels);
    void unsubscribe(std::vector<std::string> const &channels);
    void publish(std::string const &channel, std::string const &payload);

    void close();

private:

    void write(const frame& msg);

    void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void do_read_header();

    void do_read_body();

    void do_write();

    boost::asio::io_service& _ioService;
    boost::asio::ip::tcp::socket _socket;
    frame _incomingMessage;
    frame _outgoingMessage;
    frame_queue _outboundMessages;
};


#endif // __CLIENT_HPP__
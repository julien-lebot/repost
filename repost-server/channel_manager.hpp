#ifndef __CHANNEL_MANAGER_HPP__
#define __CHANNEL_MANAGER_HPP__

#include <memory>
#include <boost/asio.hpp>
#include <unordered_map>

class client;
typedef std::shared_ptr<client> client_ptr;

class channel;
typedef std::shared_ptr<channel> channel_ptr;

class channel_manager
{
private:
    std::unordered_map<std::string, channel_ptr> _channels;
public:
    void join(std::string const& channel, client_ptr client);

    void leave(client_ptr client);

    void leave(std::string const& channel, client_ptr client);

    void publish(std::string const& channel, client_ptr from, std::string const& message);
};

#endif // __CHANNEL_MANAGER_HPP__
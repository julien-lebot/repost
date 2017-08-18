#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include <memory>
#include <set>

class client;
typedef std::shared_ptr<client> client_ptr;

class channel
{
public:
    void join(client_ptr participant);

    void leave(client_ptr participant);

    void deliver(const std::string& msg, client_ptr from);

    bool is_empty() const;

private:
    std::set<client_ptr> _participants;
};

#endif // __CHANNEL_HPP__
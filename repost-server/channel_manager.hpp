#ifndef __CHANNEL_MANAGER_HPP__
#define __CHANNEL_MANAGER_HPP__

#include <memory>
#include <unordered_map>
#include <set>

namespace repost
{
    class Publication;
    class remote_client;
    typedef std::shared_ptr<remote_client> client_ptr;

    class channel_manager
    {
    private:
        typedef std::shared_ptr<std::set<client_ptr>> channel_ptr;
        std::unordered_map<std::string, channel_ptr> _channels;
    public:
        void join(std::string const& channel, client_ptr client);

        void leave(client_ptr client);

        void leave(std::string const& channel, client_ptr client);

        void publish(client_ptr from, const Publication& message);
    };
}

#endif // __CHANNEL_MANAGER_HPP__
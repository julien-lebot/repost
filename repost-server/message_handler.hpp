#ifndef __MESSAGE_HANDLER_HPP__
#define __MESSAGE_HANDLER_HPP__

#include <memory>

class frame;
class channel_manager;
class client;
typedef std::shared_ptr<client> client_ptr;

class message_handler
{
private:
    channel_manager &_channelManager;
public:
    message_handler(channel_manager& channelManager);

    void handle(const frame& frame, client_ptr client);

    void disconnect(client_ptr client);
};

#endif // __MESSAGE_HANDLER_HPP__
#include "stdafx.h"

#include <memory>
#include <boost/asio.hpp>

#include <frame.hpp>
#include <generated/cpp/repost-common/repost.pb.h>

#include "message_handler.hpp"
#include "channel_manager.hpp"

namespace repost
{
    message_handler::message_handler(channel_manager& channelManager)
        : _channelManager(channelManager)
    {
    }

    void message_handler::handle(const frame& frame, client_ptr client)
    {
        BOOST_LOG_TRIVIAL(trace) << "Handling frame of " << frame.body_length() << " bytes from " << client;

        repost::Message msg;
        msg.ParseFromArray(frame.body(), frame.body_length());

        if (msg.has_publication())
        {
            _channelManager.publish(client, msg.publication());
        }
        else if (msg.has_subscribe())
        {
            for (const auto &channel : msg.subscribe().channels())
            {
                _channelManager.join(channel, client);
            }
        }
        else if (msg.has_unsubscribe())
        {
            for (const auto &channel : msg.unsubscribe().channels())
            {
                _channelManager.leave(channel, client);
            }
        }
    }

    void message_handler::disconnect(client_ptr client)
    {
        _channelManager.leave(client);
    }
}

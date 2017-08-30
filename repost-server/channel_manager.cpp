#include "stdafx.h"
#include <generated/cpp/repost-common/repost.pb.h>
#include "channel_manager.hpp"
#include "remote_client.hpp"

namespace repost
{
    void channel_manager::join(std::string const& channelName, client_ptr client)
    {
        BOOST_LOG_TRIVIAL(info) << "Client " << client << " joined " << channelName;
        if (_channels.find(channelName) == _channels.end())
        {
            BOOST_LOG_TRIVIAL(info) << "Channel " << channelName << " doesn't exists, creating it";
            _channels[channelName] = std::make_shared<channel_ptr::element_type>();
        }
        _channels[channelName]->insert(client);
    }

    void channel_manager::leave(client_ptr client)
    {
        BOOST_LOG_TRIVIAL(info) << "Client " << client << " is disconnecting";
        std::vector<std::unordered_map<std::string, channel_ptr>::const_iterator> emptyChannels;
        for (auto channelIterator = _channels.begin(); channelIterator != _channels.end(); ++channelIterator)
        {
            channelIterator->second->erase(client);
            if (channelIterator->second->empty())
            {
                emptyChannels.push_back(channelIterator);
            }
        }
        if (!emptyChannels.empty())
        {
            BOOST_LOG_TRIVIAL(info) << "Found " << emptyChannels.size() << " empty channels, deleting them";
        }
        for (auto emptyChannel : emptyChannels)
        {
            _channels.erase(emptyChannel);
        }
    }

    void channel_manager::leave(std::string const& channelName, client_ptr client)
    {
        BOOST_LOG_TRIVIAL(info) << "Client " << client << " is leaving " << channelName;
        if (_channels.find(channelName) == _channels.end())
        {
            BOOST_LOG_TRIVIAL(warning) << "Channel " << channelName << " doesn't exists";
            return;
        }
        auto theChannel = _channels[channelName];
        theChannel->erase(client);
        if (theChannel->empty())
        {
            BOOST_LOG_TRIVIAL(info) << "Channel " << channelName << " is empty, deleting it";
            _channels.erase(channelName);
        }
    }

    void channel_manager::publish(client_ptr from, const repost::Publication& message)
    {
        BOOST_LOG_TRIVIAL(trace) << "Client " << from << " published message into " << message.channel();
        if (_channels.find(message.channel()) == _channels.end())
        {
            BOOST_LOG_TRIVIAL(warning) << "Channel " << message.channel() << " doesn't exists, discarding message from " << from;
            return;
        }
        std::string serializedMessage = message.SerializeAsString();
        for (auto &participant : *_channels[message.channel()])
        {
            if (participant != from)
            {
                participant->deliver(serializedMessage);
            }
        }
    }
}
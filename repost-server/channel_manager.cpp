#include "stdafx.h"
#include "channel_manager.hpp"
#include "channel.hpp"

void channel_manager::join(std::string const& channelName, client_ptr client)
{
    BOOST_LOG_TRIVIAL(info) << "Client " << client << " joined " << channelName;
    if (_channels.find(channelName) == _channels.end())
    {
        BOOST_LOG_TRIVIAL(info) << "Channel " << channelName << " doesn't exists, creating it";
        _channels[channelName] = std::make_shared<channel>();
    }
    _channels[channelName]->join(client);
}

void channel_manager::leave(client_ptr client)
{
    BOOST_LOG_TRIVIAL(info) << "Client " << client << " is disconnecting";
    std::vector<std::unordered_map<std::string, channel_ptr>::const_iterator> emptyChannels;
    for (auto channelIterator = _channels.begin(); channelIterator != _channels.end(); ++channelIterator)
    {
        channelIterator->second->leave(client);
        if (channelIterator->second->is_empty())
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
    auto theChannel = _channels[channelName];
    theChannel->leave(client);
    if (theChannel->is_empty())
    {
        BOOST_LOG_TRIVIAL(info) << "Channel " << channelName << " is empty, deleting it";
        _channels.erase(channelName);
    }
}

void channel_manager::publish(std::string const& channelName, client_ptr from, std::string const& message)
{
    BOOST_LOG_TRIVIAL(trace) << "Client " << from << " published message into " << channelName;
    if (_channels.find(channelName) == _channels.end())
    {
        BOOST_LOG_TRIVIAL(warning) << "Channel " << channelName << " doesn't exists, discarding message from " << from;
        return;
    }
    _channels[channelName]->deliver(message, from);
}

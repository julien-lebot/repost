#include "stdafx.h"

#include <memory>
#include <boost/tokenizer.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <google/protobuf/message_lite.h>

#include "message_handler.hpp"
#include "channel_manager.hpp"
#include <frame.hpp>
#include <generated/cpp/repost-common/repost.pb.h>

message_handler::message_handler(channel_manager& channelManager)
    : _channelManager(channelManager)
{
}

void message_handler::handle(const frame& frame, client_ptr client)
{
    BOOST_LOG_TRIVIAL(trace) << "Handling frame of " << frame.body_length() << " bytes from " << client;

    repost::Message msg;
    //google::protobuf::MessageLite

    std::string body(frame.body(), frame.body_length());
    tokenizer tok{ body, _sep};
    std::vector<std::string> tokens;
    for (const auto &token : tok)
    {
        tokens.push_back(token);
    }

    if (tokens.size() >= 2)
    {
        if (tokens.size() == 3 && boost::starts_with(tokens[0], "PUBLISH"))
        {
            const auto &channel = tokens[1];
            const auto &payload = tokens[2];
            _channelManager.publish(channel, client, payload);
        }
        else if (boost::starts_with(tokens[0], "SUBSCRIBE"))
        {
            for (auto tokenIterator = tokens.begin() + 1; tokenIterator != tokens.end(); ++tokenIterator)
            {
                _channelManager.join(*tokenIterator, client);
            }
        }
        else if (boost::starts_with(tokens[0], "UNSUBSCRIBE"))
        {
            for (auto tokenIterator = tokens.begin() + 1; tokenIterator != tokens.end(); ++tokenIterator)
            {
                _channelManager.leave(*tokenIterator, client);
            }
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(warning) << "Not enough tokens (" << tokens.size() << ") in frame";
    }
}

void message_handler::disconnect(client_ptr client)
{
    _channelManager.leave(client);
}

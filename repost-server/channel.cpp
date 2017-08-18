#include "stdafx.h"
#include "channel.hpp"
#include "client.hpp"

void channel::join(client_ptr participant)
{
    _participants.insert(participant);
}

void channel::leave(client_ptr participant)
{
    _participants.erase(participant);
}

void channel::deliver(const std::string& msg, client_ptr from)
{
    for (auto participant : _participants)
    {
        if (participant != from)
        {
            participant->deliver(msg);
        }
    }
}

bool channel::is_empty() const
{
    return _participants.empty();
}

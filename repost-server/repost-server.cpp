// repost-server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class chat_server
{
public:
    chat_server(boost::asio::io_service& io_service,
        const tcp::endpoint& endpoint)
        : acceptor_(io_service, endpoint),
        socket_(io_service)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_,
            [this](boost::system::error_code ec)
        {
            if (!ec)
            {
                //std::make_shared<chat_session>(std::move(socket_), room_)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    //chat_room room_;
};

int main()
{
    return 0;
}


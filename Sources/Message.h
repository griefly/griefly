#pragma once

#include <Poco/Net/SocketStream.h>
#include <string>

struct Message
{
    Message();
    Message(const std::string& text);
    std::string text;

    // TODO: type

    int from;
    int to;

    std::string type;
    int message_number;
};

bool SendSocketMessage(Poco::Net::StreamSocket& socket, const Message& message);
bool RecvSocketMessage(Poco::Net::StreamSocket& socket, Message* message);

#include "Message.h"

#include "constheader.h"

#include <sstream>

const char* const ORDINARY_TYPE = "ordinary";

Message::Message()
    : text(""),
    message_number(-1),
    from(-1),
    to(-1),
    type(ORDINARY_TYPE) {}

Message::Message(const std::string& new_text)
    : text(new_text),
    message_number(-1),
    from(-1),
    to(-1),
    type(ORDINARY_TYPE) {}

bool SocketReady(TCPsocket& socket)
{
    static SDLNet_SocketSet set = SDLNet_AllocSocketSet(1); 
    SDLNet_TCP_AddSocket(set, socket);
    bool retval = (SDLNet_CheckSockets(set, 0) != 0);
    SDLNet_TCP_DelSocket(set, socket);
    return retval;
}

bool SendSocketMessage(TCPsocket& socket, const Message& message)
{
    std::stringstream convertor;
    convertor << message.message_number << " " 
              << message.from << " "
              << message.to << " "
              << message.type << " "
              << message.text;
    
    std::string tail = convertor.str();

    SYSTEM_STREAM << "Message send " << message.type << " size: " << tail.length() << std::endl;

    convertor.str("");
    convertor << tail.length() << " ";

    std::string sendval = convertor.str();
    sendval.append(tail);

    size_t pos = 0;
    const char* begin_pos = sendval.c_str();
    size_t length = sendval.length();
    SYSTEM_STREAM << "Sendval.length " << sendval.length() << std::endl;
    if (SDLNet_TCP_Send(socket, begin_pos, length) != length)
    {
        SYSTEM_STREAM << SDLNet_GetError() << std::endl;
        return false;
    }
    return true;
}

bool RecvAllMessage(TCPsocket& socket, char* ptr, int length)
{
    int retval;
    while ((retval = SDLNet_TCP_Recv(socket, ptr, length)) != length)
    {
        if (retval <= 0)
            return false;
        ptr += retval;
        length -= retval;
    }
    return true;
}

bool RecvSocketMessage(TCPsocket& socket, Message* message)
{
    message->text.clear();

    std::stringstream convertor;
    std::string number;

    char local_char = '`'; // Just symbol
    while (local_char != ' ')
    {
        if (!RecvAllMessage(socket, &local_char, 1))
        {
            SYSTEM_STREAM << "Fail read message length" << std::endl;
            SYSTEM_STREAM << SDLNet_GetError() << std::endl;
            return false;
        };
        number.append(&local_char, 1);
    }

    size_t length;

    convertor << number;
    convertor >> length;

    const size_t MAX_SIZE_MESSAGE = 1024 * 1024 * 8;
    if (convertor.fail() || length > MAX_SIZE_MESSAGE)
    {
        SYSTEM_STREAM << "Fail read number: not a number or too big" << std::endl;
        return false;
    }

    convertor.str("");

    char* raw_message = new char[length];

    if (!RecvAllMessage(socket, raw_message, length))
    {
        SYSTEM_STREAM << "Error receive byted: " << std::endl; 
        SYSTEM_STREAM << SDLNet_GetError() << std::endl;
        delete[] raw_message;
        return false;
    }

    std::string string_message(raw_message, length);
    delete[] raw_message;

    // SYSTEM_STREAM << "Raw message received: " << string_message << std::endl;

    size_t itr = 0;
    int counter = 0;
    while (itr != string_message.length() && counter != 4)
        if (string_message[itr++] == ' ')
            ++counter;
    if (itr == string_message.length())
    {
        SYSTEM_STREAM << "Wrong message receive: " << string_message << std::endl;
        return false;
    }

    message->text = string_message.substr(itr);

    convertor << string_message.substr(0, itr);

    convertor >> message->message_number;
    convertor >> message->from;
    convertor >> message->to;
    convertor >> message->type;
   

    if (convertor.fail())
    {
        SYSTEM_STREAM << "Bad message receive" << std::endl;
        return false;
    }

    /*SYSTEM_STREAM 
        << "Received message : \n"
        << "number: " << message->message_number
        << "from: "   << message->from
        << "to: "     << message->to
        << "type: "   << message->type
        << "text: "   << message->text;
        */
    return true;
}
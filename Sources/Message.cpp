#include "Message.h"

#include "constheader.h"

#include <sstream>
#include <Poco\Exception.h>

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

bool SendSocketMessage(Poco::Net::StreamSocket& socket, const Message& message)
{
    std::stringstream convertor;
    convertor << message.message_number << " " 
              << message.from << " "
              << message.to << " "
              << message.type << " "
              << message.text;
    
    std::string tail = convertor.str();

    convertor.str("");
    convertor << tail.length() << " ";

    std::string sendval = convertor.str();
    sendval.append(tail);

    size_t pos = 0;
    const char* begin_pos = sendval.c_str();
    size_t length = sendval.length();
    try
    {
        while (pos != length)
            pos += socket.sendBytes(begin_pos + pos, length - pos);
        return true;
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << "Error send bytes: " << std::endl;
        SYSTEM_STREAM << e.displayText() << std::endl;
        return false;
    }
}

bool RecvSocketMessage(Poco::Net::StreamSocket& socket, Message* message)
{
    message->text.clear();

    std::stringstream convertor;
    std::string number;

   // SYSTEM_STREAM << "Begin receive message length" << std::endl;

    char c = '`'; // Just symbol
    try
    {
        while (c != ' ')
        {
            while (socket.receiveBytes(&c, 1) == 0) {};

            number.append(&c, 1);
        }
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << std::endl;
        return false;
    }

   // SYSTEM_STREAM << "Length of message received: " << number << std::endl;

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

   // SYSTEM_STREAM << "Begin main message receive" << std::endl;

    size_t pos = 0;
    try
    {
        while (pos != length)
            pos += socket.receiveBytes(raw_message + pos, length - pos);
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << "Error receive byted: " << std::endl; 
        SYSTEM_STREAM << e.displayText() << std::endl;
        delete[] raw_message;
        return false;
    }

    std::string string_message(raw_message, length);
    delete[] raw_message;

    size_t itr = 0;
    int counter = 0;
    while (itr != string_message.length() && counter != 4)
        if (string_message[itr++] == ' ')
            ++counter;
    if (itr == string_message.length())
    {
        SYSTEM_STREAM << "Wrong message receive" << std::endl;
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
#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <string>
#include <vector>
#include <boost/asio/streambuf.hpp>


namespace srv {
namespace message {

    /**
     *
     */
    class Message {
    public:
        typedef std::pair<std::string, std::string> FDirective;
        typedef std::vector<FDirective> FDirectives;

    public:
        Message()               = default;
        Message(const Message&) = default;
        Message(const std::string&      MSG);
        Message(boost::asio::streambuf& MSG);
        virtual ~Message() = default;

    public:
        std::string Method;
        std::string Protocol;

        std::string Path;
        FDirective  Code;
        FDirectives Directives;
        std::string Body;

    public:
        bool isReponse();

    public:
        Message& SetMethod  (std::string Method);
        Message& SetProtocol(std::string Protocol);

        Message& SetCode(const FDirective& Code);
        Message& SetCode(const std::string Code, const std::string Description);

        Message& SetDirective(const FDirective &Directive);
        Message& SetDirective(const std::string Name, const std::string Value);

        Message& SetData(const std::string& MSG);

    public:
        std::string GetDirective(std::string Name);
        std::string GetHeader();
        std::string GetMessage();

    protected:
        std::string Hat();
        std::string RequestHat();
        std::string ResponceHat();

        void ParsHat   (const std::string& Hat);
        void ParsHeader(const std::string& Header);
    };

}
}

std::ostream&           operator<<(std::ostream& os,           srv::message::Message& msg);
boost::asio::streambuf& operator<<(boost::asio::streambuf& sb, srv::message::Message& msg);

#endif //SERVER_MESSAGE_H

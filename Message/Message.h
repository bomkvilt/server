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
    class AMessage {
    public:
        typedef std::pair<std::string, std::string> FDirective;
        typedef std::vector<FDirective> FDirectives;

    public:
        AMessage()               = default;
        AMessage(const AMessage&) = default;
        AMessage(const std::string&      MSG);
        AMessage(boost::asio::streambuf& MSG);
        virtual ~AMessage() = default;

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
        AMessage& SetMethod  (std::string Method);
        AMessage& SetProtocol(std::string Protocol);

        AMessage& SetCode(const FDirective& Code);
        AMessage& SetCode(const std::string Code, const std::string Description);

        AMessage& SetDirective(const FDirective &Directive);
        AMessage& SetDirective(const std::string Name, const std::string Value);

        AMessage& SetData(const std::string& MSG);

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

std::ostream&           operator<<(std::ostream& os,           srv::message::AMessage& msg);
boost::asio::streambuf& operator<<(boost::asio::streambuf& sb, srv::message::AMessage& msg);

#endif //SERVER_MESSAGE_H

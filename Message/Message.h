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
        typedef std::pair<std::string, std::string> LDirective;
        typedef std::vector<LDirective>             LDirectives;

    public:
        AMessage()                = default;
        AMessage(const AMessage&) = default;
        AMessage(const std::string&      MSG);
        AMessage(boost::asio::streambuf& MSG);
        AMessage(const std::string& Code, const std::string Description, bool bClose = false);
        virtual ~AMessage() = default;

    public:
        std::string Method;
        std::string Protocol;

        std::string Path;
        LDirective  Code;
        LDirectives Directives;
        std::string Body;

    public:
        bool isReponse();

    public:
        AMessage& SetMethod  (std::string Method);
        AMessage& SetProtocol(std::string Protocol);

        AMessage& SetCode(const LDirective& Code);
        AMessage& SetCode(const std::string Code, const std::string Description);

        AMessage& SetDirective(const LDirective &Directive);
        AMessage& SetDirective(const std::string Name, const std::string Value);

        AMessage& SetData(const std::string& MSG);

    public:
        std::string& GetDirective(std::string Name);
        std::string  GetHeader();
        std::string  GetMessage();

    protected:
        std::string Hat();
        std::string RequestHat();
        std::string ResponceHat();
        std::string GetLine(const LDirective& d);

        void do_preprocess();
        void ParsHat   (const std::string& Hat);
        void ParsHeader(const std::string& Header);

    public:
        std::string& operator[](const std::string& Name);
    };

    // 200 OK
    AMessage SuccessMessage(bool bClose = true);

    // 404 NotFound
    AMessage NotFoundMessage();
}
}

std::ostream&           operator<<(std::ostream& os,           srv::message::AMessage& msg);
boost::asio::streambuf& operator<<(boost::asio::streambuf& sb, srv::message::AMessage& msg);

#endif //SERVER_MESSAGE_H

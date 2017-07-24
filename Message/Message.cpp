
#include <regex>
#include <iostream>
#include "Message.h"


using namespace srv::message;

Message::Message(const std::string& MSG) {
    SetData(MSG);
}

Message::Message(boost::asio::streambuf& MSG) {
    std::istream in(&MSG);
    in >> std::noskipws;

    std::string tmp;
    std::copy(std::istream_iterator<char>(in)
            , std::istream_iterator<char>()
            , std::back_inserter(tmp)
    );
    SetData(tmp);
}



bool Message::isReponse() {
    return Method.size() == 0;
}

Message &Message::SetMethod(std::string Method) {
    this->Method = Method;
    return *this;
}

Message &Message::SetProtocol(std::string Protocol) {
    this->Protocol = Protocol;
    return *this;
}

Message& Message::SetCode(const Message::FDirective& Code) {
    return SetCode(Code.first, Code.second);
}

Message& Message::SetCode(const std::string Code, const std::string Description) {
    this->Code.first  = Code;
    this->Code.second = Description;
    return *this;
}

Message &Message::SetDirective(const Message::FDirective &Directive) {
    return SetDirective(Directive.first, Directive.second);
}

Message& Message::SetData(const std::string& MSG) {
    std::stringstream in(MSG);
    in >> std::noskipws;
    std::string Header;
    std::string Hat;
//hat
    std::getline(in, Hat);
    Hat += '\n';
    ParsHat(Hat);
//header
    std::string buff;
    while(std::getline(in, buff) && (buff != "\r" || buff != "")) {
        Header += buff + "\n";
        buff = "";
    } ParsHeader(Header);
//body
    std::copy(std::istream_iterator<char>(in)
            , std::istream_iterator<char>()
            , std::back_inserter(Body)
    );
    return *this;
}

Message &Message::SetDirective(const std::string Name, const std::string Value) {
    for (auto& itr : Directives)
        if (itr.first == Name) {
            itr.second = Value;
            return *this;
        }
    Directives.emplace_back(Name, Value);
    return *this;
}



std::string Message::GetDirective(std::string Name) {
    for (auto& itr : Directives)
        if (itr.first == Name)
            return itr.second;
    return "";
}

std::string Message::GetHeader() {
    std::string Header =  Hat();
    for (auto& itr : Directives)
        Header += itr.first + ": " + itr.second + "\r\n";
    Header += "\r\n";
    return Header;
}

std::string Message::GetMessage() {
    return GetHeader() + Body;
}

std::string Message::Hat() {
    if (isReponse())
        return ResponceHat();
    return RequestHat();
}

std::string Message::RequestHat() {
    return Method       + " "
           + Path       + " "
           + Protocol   + "\r\n";
}

std::string Message::ResponceHat() {
    return Protocol      + " "
           + Code.first  + " "
           + Code.second + "\r\n";
}

#define SPACE "[[:s:]]+"
#define LINE  "\r?\n"

void Message::ParsHat(const std::string& Hat) {
    //                         POST                 path               HTTP/1.1
    static std::regex ein  ("([A-Z]+)"    SPACE "([^[:s:]]+)" SPACE "([^[:s:]]+)" LINE);
    static std::regex eout ("([^[:s:]]+)" SPACE "([[:d:]]+)"  SPACE "([^[\r\n]])" LINE);
    //                        HTTP/1.1              200                   OK

    std::smatch m;
    if (std::regex_match(Hat, m, ein)) {
        Method   = m[1].str();
        Path     = m[2].str();
        Protocol = m[3].str();
    } else
    if (std::regex_match(Hat, m, eout)) {
        Protocol    = m[1].str();
        Code.first  = m[2].str();
        Code.second = m[3].str();
    }
}

void Message::ParsHeader(const std::string& Header) {
    //                     Expires           Sat, 28 Nov 2009 05:36:25 GMT
    static std::regex e("([^[:s:]]+)" ":" SPACE "([^[:s:]^\r^\n]+)" LINE);

    std::sregex_iterator itr(Header.begin(), Header.end(), e);
    std::sregex_iterator end;
    for (; itr != end; ++itr)
        Directives.emplace_back(itr->str(1), itr->str(2));
}

std::ostream& operator<<(std::ostream &os, Message& msg) {
    os << msg.GetMessage();
    return os;
}

boost::asio::streambuf& operator<<(boost::asio::streambuf& sb, Message& msg) {
    std::ostream os(&sb);
    os << msg.GetMessage();
    return sb;
}

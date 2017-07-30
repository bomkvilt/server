
#include <regex>
#include <iostream>
#include "Message.h"
#include "../Service.h"


using namespace srv::message;

AMessage::AMessage(const std::string& MSG) {
    SetData(MSG);
}

AMessage::AMessage(boost::asio::streambuf& MSG) {
    std::istream in(&MSG);
    in >> std::noskipws;

    std::string tmp;
    std::copy(std::istream_iterator<char>(in)
            , std::istream_iterator<char>()
            , std::back_inserter(tmp)
    );
    SetData(tmp);
}

AMessage::AMessage(const std::string& Code, const std::string Description, bool bClose)
    : Code(Code, Description)
{
    if (bClose) { (*this)["Connection"] = "close";      }
    else        { (*this)["Connection"] = "Keep-Alive"; }
}


bool AMessage::isReponse() {
    return Method.size() == 0;
}

AMessage &AMessage::SetMethod(std::string Method) {
    this->Method = Method;
    return *this;
}

AMessage &AMessage::SetProtocol(std::string Protocol) {
    this->Protocol = Protocol;
    return *this;
}

AMessage& AMessage::SetCode(const AMessage::LDirective& Code) {
    return SetCode(Code.first, Code.second);
}

AMessage& AMessage::SetCode(const std::string Code, const std::string Description) {
    this->Code.first  = Code;
    this->Code.second = Description;
    return *this;
}

AMessage &AMessage::SetDirective(const AMessage::LDirective &Directive) {
    return SetDirective(Directive.first, Directive.second);
}

AMessage& AMessage::SetData(const std::string& MSG) {
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
    while(std::getline(in, buff)) {
        checkB(buff != "\r" && buff != "");
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

AMessage &AMessage::SetDirective(const std::string Name, const std::string Value) {
    for (auto& itr : Directives)
        if (itr.first == Name) {
            itr.second = Value;
            return *this;
        }
    Directives.emplace_back(Name, Value);
    return *this;
}



std::string& AMessage::GetDirective(std::string Name) {
    for (auto& itr : Directives)
        if (itr.first == Name)
            return itr.second;
    Directives.emplace_back(Name, "");
    return Directives.back().second;
}

std::string AMessage::GetHeader() {
    do_preprocess();

    std::string Header =  Hat();
    for (auto& itr : Directives) {
        if (itr.second.length())
            Header += GetLine(itr);
    }
    Header += "\r\n";
    return Header;
}

std::string AMessage::GetMessage() {
    if (Method == "HEAD")
        return GetHeader();
    return GetHeader() + Body;
}

std::string AMessage::Hat() {
    if (isReponse())
        return ResponceHat();
    return RequestHat();
}

std::string AMessage::RequestHat() {
    return Method       + " "
           + Path       + " "
           + Protocol   + "\r\n";
}

std::string AMessage::ResponceHat() {
    return Protocol      + " "
           + Code.first  + " "
           + Code.second + "\r\n";
}

std::string AMessage::GetLine(const LDirective& d) {
    return d.first + ": " + d.second + "\r\n";
}

void AMessage::do_preprocess() {
    auto& length = GetDirective("Content-Length");
    if (!length.length())
        length = std::to_string(Body.length());
}

#define SPACE "[[:s:]]+"
#define LINE  "\r?\n"

void AMessage::ParsHat(const std::string& Hat) {
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

void AMessage::ParsHeader(const std::string& Header) {
    //                     Expires           Sat, 28 Nov 2009 05:36:25 GMT
    static std::regex e("([^[:s:]]+)" ":" SPACE "([^[:s:]^\r^\n]+)" LINE);

    std::sregex_iterator itr(Header.begin(), Header.end(), e);
    std::sregex_iterator end;
    for (; itr != end; ++itr)
        Directives.emplace_back(itr->str(1), itr->str(2));
}

std::string& AMessage::operator[](const std::string& Name) {
    return GetDirective(Name);
}

std::ostream& operator<<(std::ostream &os, AMessage& msg) {
    os << msg.GetMessage();
    return os;
}

boost::asio::streambuf& operator<<(boost::asio::streambuf& sb, AMessage& msg) {
    std::ostream os(&sb);
    os << msg.GetMessage();
    return sb;
}

AMessage srv::message::SuccessMessage(bool bClose) {
    AMessage Message("200", "OK", bClose);
    return Message;
}

AMessage srv::message::NotFoundMessage() {
    AMessage Message("404", "Not found", true);
    return Message;
}

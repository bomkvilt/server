
#include <fstream>
#include <iterator>
#include "../Server.h"
#include "Location.h"

using namespace srv::location;
using namespace srv;


message::AMessage ALocation::ProcessMessage(const message::AMessage &Message) {
    switch (ResultType) {
        CASER(LORT_ERROR)       on_error  (Message);
        CASER(LORT_FILE)        on_file   (Message);
        CASER(LORT_APP)         on_app    (Message);
        CASER(LORT_CONTROLL)    on_control(Message);
    }
    throw std::runtime_error(
            " ## Unknown type of Message type; at " + __LINE__
    );
}

PTR(AServer) ALocation::Server() {
    return server.lock();
}

PTR(MIME_Detector) ALocation::MIME() {
    checkE(Server())
        throw std::runtime_error(
                " ## Empty reference on server; at " + __LINE__
        );
    return Server()->GetConfig().MIME;
}

std::string ALocation::BaseRoot() {
    checkE(Server())
        throw std::runtime_error(
                " ## Empty reference on server; at " + __LINE__
        );
    return Server()->GetConfig().WorkPath;
}

std::string ALocation::GetClearPath(std::string Path) {
    auto MItr = Path.begin();
    auto MEnd = Path.end();
    auto EItr = Expr.begin();
    auto EEnd = Expr.end();
    while (MItr != MEnd && EItr != EEnd) {
        if (*MItr != *EItr)
            break;
        ++MItr; ++EItr;
    }

    std::string Clear;
    std::copy(MItr, MEnd
            , std::back_inserter(Clear));
    return Clear;
}

message::AMessage ALocation::on_error(const message::AMessage &Message) {
    message::AMessage tmp;
    tmp.Code.first  = "404";
    tmp.Code.second = "Not found";
    tmp.Body = "";
    return tmp;
}

message::AMessage ALocation::on_file(const message::AMessage &Message) {
    std::string Path = BaseRoot() + Root + GetClearPath(Message.Path);
    SWITCH(Message.Method) {
        CASERM("GET")   on_file_r(Message, Path);
        CASERM("POST")  on_file_w(Message, Path);
    }}
return on_error(Message);
}

message::AMessage ALocation::on_file_r(const message::AMessage& Message, const std::string& Path) {
    std::ifstream in(Path);
    in >> std::noskipws;
    check(in.is_open() && Server())
                on_error(Message);

    message::AMessage Reponse;
    std::copy(std::istream_iterator<char>(in)
            , std::istream_iterator<char>()
            , std::back_inserter(Reponse.Body)
    );

    Reponse .SetCode     ("200","OK")
            .SetProtocol ("HTTP/1.1")
            .SetDirective("Content-Length",      std::to_string(Reponse.Body.size()))
            .SetDirective("Content-Disposition", "attachment; filename=\"" + MIME()->GetName(Message.Path) + "\"")
            .SetDirective("Content-Type",        MIME()->GetMIME(Message.Path))
            .SetDirective("Connection",          "close");
    return Reponse;
}

message::AMessage ALocation::on_file_w(const message::AMessage& Message, const std::string& Path) {
    std::ofstream out(Path);
    out << std::noskipws;
    check(out.is_open() && Server())
                on_error(Message);

    std::copy(Message.Body.begin()
            , Message.Body.end()
            , std::ostream_iterator<char>(out));

    message::AMessage Reponse;
    Reponse .SetCode     ("200","OK")
            .SetProtocol ("HTTP/1.1")
            .SetDirective("Content-Length",      std::to_string(Reponse.Body.size()))
            .SetDirective("Connection",          "close");
    return Reponse;
}

message::AMessage ALocation::on_app(const message::AMessage &Message) {
    check(AppBack)
        on_error(Message);
    return AppBack(Message);
}

message::AMessage ALocation::on_control(const message::AMessage& Message) {
    check(ControlBack && Server())
        on_error(Message);
    return ControlBack(Message, Server());
}
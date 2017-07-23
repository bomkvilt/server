
#include <fstream>
#include <iterator>
#include "Location.h"

using namespace srv::location;
using namespace srv;


message::Message Location::ProcessMessage(const message::Message &Message) {
    switch (ResultType) {
        CASER(LORT_ERROR)   on_error(Message);
        CASER(LORT_FILE)    on_file (Message);
        CASER(LORT_APP)     on_app  (Message);
    }
}

message::Message Location::on_error(const message::Message &Message) {
    message::Message tmp;
    tmp.Code.first  = "404";
    tmp.Code.second = "Not found"; //TODO::common code header
    tmp.Body = "";
    return tmp;
}

message::Message Location::on_file(const message::Message &Message) {
    const std::string Path = BaseRoot + Root + Message.Path;
    std::ifstream in(Path);
    in >> std::noskipws;
    if (!in.is_open() || !MIME)
        return on_error(Message);

    auto Reponse = message::Message();
    std::copy(std::istream_iterator<char>(in),
              std::istream_iterator<char>(),
              std::back_inserter(Reponse.Body)
    );

    Reponse .SetCode     ("200", "OK")
            .SetProtocol ("HTTP/1.1")
            .SetDirective("Content-Length",      std::to_string(Reponse.Body.size()))
            .SetDirective("Content-Disposition", "attachment; filename=\"" + MIME->GetName(Message.Path) + "\"")
            .SetDirective("Content-Type",        MIME->GetMIME(Message.Path))
            .SetDirective("Connection",          "close");
    return Reponse;
}

message::Message Location::on_app(const message::Message &Message) {
    if (!AppBack)
        return on_error(Message);
    return AppBack(Message);
}
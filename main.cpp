#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "Server.h"


using namespace srv;

message::AMessage ExampleApp(const message::AMessage& MSG) {
    auto Reponse = message::AMessage();
    Reponse.Body = "U required page " + MSG.Path + ".\r\n"
                   "We sorry, but the page is not available";

    Reponse .SetCode     ("200","OK")
            .SetProtocol ("HTTP/1.1")
            .SetDirective("Content-Length",      std::to_string(Reponse.Body.size()))
            .SetDirective("Content-Disposition", "filename=\"Test.html\"")
            .SetDirective("Content-Type",        "text/html")
            .SetDirective("Connection",          "close");
    return Reponse;
}

message::AMessage ExampleControl(const message::AMessage& MSG, PTR(AServer) Server) {
    Server->GetConfig().Port = 8002;
    Server->Refresh();

    message::AMessage Reponse;
    Reponse.Body = "Migrated to port 8002";
    Reponse .SetCode    ("200", "OK")
            .SetProtocol("HTTP/1.1")
            .SetDirective("Content-Length",      std::to_string(Reponse.Body.size()))
            .SetDirective("Content-Disposition", "filename=\"Port.html\"")
            .SetDirective("Content-Type",        "text/html")
            .SetDirective("Connection",          "close");
    return Reponse;
}

int main(int argc, char* argv[]) {
    // parse arguments
    if (argc > 2) {
        std::cout << argv[0] << " [port = 8001]" << std::endl;
        return 1;
    }
    uint16_t Port = argc == 1 ? (uint16_t)8001 : boost::lexical_cast<uint16_t>(argv[1]);

    // file server location
    location::ALocation FileLocation;
    FileLocation.ResultType = location::LORT_FILE;
    FileLocation.ExprType   = location::LEXT_PREFIX;
    FileLocation.Expr       = "/get";
    FileLocation.Root       = "";

    // application server location
    location::ALocation AppLocation;
    AppLocation.ResultType = location::LORT_APP;
    AppLocation.ExprType   = location::LEXT_PREFIX;
    AppLocation.Expr       = "/app";
    AppLocation.AppBack    = MEM_FF1(ExampleApp, _1);

    // application server location
    location::ALocation ControlLocation;
    ControlLocation.ResultType  = location::LORT_CONTROLL;
    ControlLocation.ExprType    = location::LEXT_PREFIX;
    ControlLocation.Expr        = "/admin";
    ControlLocation.ControlBack = MEM_FF2(ExampleControl, _1, _2);

    // create server
    auto server = AServer::Create();
    server->GetConfig().Locations.SetLocation(FileLocation);
    server->GetConfig().Locations.SetLocation(AppLocation);
    server->GetConfig().Locations.SetLocation(ControlLocation);
    server->GetConfig().WorkPath  = "/home/kvilt/Downloads";
    server->GetConfig().MIME_Path = "/home/kvilt/Downloads/MIME.txt";
    server->GetConfig().Port      = Port;

    // launch one
    boost::thread t ( [&server](){
        server->Start();
    });
    t.detach();

    // waiting for a stop
    std::string buff;
    while(true) {
        getline(std::cin, buff);
        if (buff == "stop") {
            server->Stop();
            boost::this_thread::sleep(
                boost::posix_time::millisec(100)
            );
            break;
        }
        buff = "";
    }


    return 0;
}
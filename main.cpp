#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "Server.h"


using namespace srv;

message::Message ExampleApp(const message::Message& MSG) {
    auto Reponse = message::Message();
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

int main(int argc, char* argv[]) {
    // parse arguments
    if (argc > 2) {
        std::cout << argv[0] << " [port = 8001]" << std::endl;
        return 1;
    }
    uint16_t Port = argc == 1 ? (uint16_t)8001 : boost::lexical_cast<uint16_t>(argv[1]);

    // file server location
    location::Location FileLocation;
    FileLocation.ResultType = location::LORT_FILE;
    FileLocation.ExprType   = location::LEXT_PREFIX;
    FileLocation.Expr       = "/get";

    // application server location
    location::Location AppLocation;
    AppLocation.ResultType = location::LORT_APP;
    AppLocation.ExprType   = location::LEXT_PREFIX;
    AppLocation.Expr       = "/app";
    AppLocation.AppBack    = MEM_FF1(ExampleApp, _1);

    // create server
    auto server = Server::Create();
    server->GetConfig().Locations.SetLocation(FileLocation);
    server->GetConfig().Locations.SetLocation(AppLocation);
    server->GetConfig().WorkPath    = "/home/kvilt/Downloads";
    server->GetConfig().MIME_Path   = "/home/kvilt/Downloads/MIME.txt";
    server->GetConfig().Port        = Port;

    // launch one
    boost::thread t ( [&server](){
        server->Start();
    });
    t.detach();

    // waiting for stop
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
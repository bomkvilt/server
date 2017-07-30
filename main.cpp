#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "Server.h"


using namespace srv;



int main(int argc, char* argv[]) {

    // file server location
    location::ALocation FileLocation;
    FileLocation.ResultType = location::LORT_FILE;
    FileLocation.ExprType   = location::LEXT_PREFIX;
    FileLocation.Expr       = "/get";
    FileLocation.Root       = "";


    // create server
    auto server = AServer::Create();
    server->GetConfig().Locations
            .SetLocation(FileLocation);
    server->GetConfig().WorkPath  = "/home/kvilt/Downloads";
    server->GetConfig().MIME_Path = "/home/kvilt/Downloads/MIME.txt";
    server->GetConfig().Port      = 8001;




    /*******************|  |*******************/

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
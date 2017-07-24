#include <algorithm>
#include "Server.h"
#include "Service.h"

using namespace srv;

ServerConfig::ServerConfig() :
    bStarted        (false),
    //bAccept         (true),
    Port            (8001),
    WorkPath        (""),
    MIME_Path       (""),
    AccessLog        (std::cout),
    ErrorLog        (std::cerr),
    MIME            (nullptr)
    //MaxConections   (1024)
{
    MIME = MIME_Detector::Create();
}


Server::Server() :
    config(),
    service(),
    acceptor(service)
{}

Server::ptr Server::Create() {
    return ptr(new Server());
}

Server::~Server() {
    Stop();
}

/****************************************|  |****************************************/

void Server::Start() {
    config.AccessLog << "Server is started "
                    << std::endl;
    config.bStarted = 1;
    update_dependencies();
    do_accept();
    service.run();
}

void Server::Stop() {
    check(config.bStarted);
    config.AccessLog << "Server is stopping..";

    config.bStarted = 0;
    for (auto& i : clients)
        i->Stop();
    service.stop();
    acceptor.cancel();

    config.AccessLog << " OK"
                    << std::endl;
}

/****************************************|  |****************************************/

ServerConfig& Server::GetConfig() {
    return config;
}

size_t Server::ClientCount() const {
    return clients.size() - 1;
}

/****************************************|  |****************************************/

Server::client_wptr Server::NewClient() {
    auto tmp = ClientConnection::Create(
            service,
            config,
            MEM_FN1(HandleUnbind, _1)
    );
    clients.push_back(tmp);
    return clients.back()->weak_from_this();
}

/****************************************|  |****************************************/

void Server::HandleAccept(Server::client_wptr Client, const Server::ErrorCode &Err) {
    config.AccessLog << " -- Client connected"
                    << std::endl;
    Client.lock()->Start();
    do_accept();
}

void Server::HandleUnbind(Server::client_wptr Client) {
    auto shared = Client.lock();
    auto itr    = std::find(clients.begin(), clients.end(), shared);
    clients.erase(itr);

    config.AccessLog << " -- Client disconnected; "
                    << "clients: " << ClientCount()
                    << std::endl;
}

void Server::do_accept() {
    auto Client = NewClient();
    acceptor.async_accept(
            Client.lock()->Socket(),
            MEM_FN2(HandleAccept, Client, _1)
    );
}

void Server::update_dependencies() {
    config.MIME->UpdateBase(config.MIME_Path);
    config.Locations.SetMIME(config.MIME);
    config.Locations.SetRoot(config.WorkPath);
    acceptor = ip::tcp::acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), config.Port));
}

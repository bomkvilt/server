#include <algorithm>
#include "Server.h"
#include "Service.h"

using namespace srv;

ServerConfig::ServerConfig() :
    bStarted        (false),
    bAccept         (true),
    Port            (8001),
    WorkPath        (""),
    MIME_Path       (""),
    AcessLog        (std::cout),
    ErrorLog        (std::cerr),
    MIME            (nullptr),
    MaxConections   (1024)
{
    MIME = MIME_Detector::Create();
}


Server::Server() :
    states(),
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
    states.AcessLog << "Server is started "
                    << std::endl;
    states.bStarted = 1;
    update_dependeces();
    do_accept();
    service.run();
}

void Server::Stop() {
    check(states.bStarted);
    states.AcessLog << "Server is stopping..";

    states.bStarted = 0;
    for (auto& i : clients)
        i->Stop();
    service.stop();
    acceptor.cancel();

    states.AcessLog << " OK"
                    << std::endl;
}

/****************************************|  |****************************************/

ServerConfig& Server::GetConfig() {
    return states;
}

size_t Server::ClientCount() const {
    return clients.size() - 1;
}

/****************************************|  |****************************************/

Server::client_wptr Server::NewClient() {
    auto tmp = ClientConnection::Create(
            service,
            states,
            MEM_FN1(HandleUnbind, _1)
    );
    clients.push_back(tmp);
    return clients.back()->weak_from_this();
}

/****************************************|  |****************************************/

void Server::HandleAccept(Server::client_wptr Client, const Server::ErrorCode &Err) {
    states.AcessLog << " -- Client connected"
                    << std::endl;
    Client.lock()->Start();
    do_accept();
}

void Server::HandleUnbind(Server::client_wptr Client) {
    auto shared = Client.lock();
    auto itr    = std::find(clients.begin(), clients.end(), shared);
    clients.erase(itr);

    states.AcessLog << " -- Client disconnected; "
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

void Server::update_dependeces() {
    states.MIME->UpdateBase(states.MIME_Path);
    states.Locations.SetMIME(states.MIME);
    states.Locations.SetRoot(states.WorkPath);
    acceptor = ip::tcp::acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), states.Port));
}

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
    AccessLog       (std::cout),
    ErrorLog        (std::cerr),
    MIME            (nullptr)
    //MaxConections   (1024)
{
    MIME = MIME_Detector::Create();
}


AServer::AServer() :
    config(),
    service(),
    acceptor(service)
{}

AServer::ptr AServer::Create() {
    return ptr(new AServer());
}

AServer::~AServer() {
    Stop();
}

/****************************************|  |****************************************/

void AServer::Start() {
    config.AccessLog << "Server is started "
                    << std::endl;
    config.bStarted = 1;
    update_dependencies();
    do_accept();
    service.run();
}

void AServer::Stop() {
    check(config.bStarted);
    config.AccessLog << "Server is stopping.."
                     << std::endl;

    config.bStarted = 0;
    RemoveClients();
    service.stop();
    acceptor.cancel();

    config.AccessLog << "Server stopped"
                    << std::endl;
}

void AServer::Refresh() {
    update_dependencies();
    config.AccessLog << "Server updated"
                     << std::endl;
}

void AServer::RemoveClients() {
    while (clients.size())
        clients.back()->Stop();
}

/****************************************|  |****************************************/

ServerConfig& AServer::GetConfig() {
    return config;
}

size_t AServer::ClientCount() const {
    return clients.size() - 1;
}

/****************************************|  |****************************************/

AServer::client_wptr AServer::NewClient() {
    auto tmp = ClientConnection::Create(
            service,
            config,
            MEM_FC1(HandleUnbind, _1)
    );
    clients.push_back(tmp);
    return clients.back()->weak_from_this();
}

/****************************************|  |****************************************/

void AServer::HandleAccept(AServer::client_wptr Client, const AServer::ErrorCode &Err) {
    config.AccessLog << " -- Client connected"
                    << std::endl;
    Client.lock()->Start();
    do_accept();
}

void AServer::HandleUnbind(AServer::client_wptr Client) {
    auto shared = Client.lock();
    auto itr    = std::find(clients.begin(), clients.end(), shared);
    clients.erase(itr);

    config.AccessLog << " -- Client disconnected; "
                    << "clients: " << ClientCount()
                    << std::endl;
}

void AServer::do_accept() {
    auto Client = NewClient();
    acceptor.async_accept(
            Client.lock()->Socket(),
            MEM_FC2(HandleAccept, Client, _1)
    );
}

void AServer::update_dependencies() {
    config.MIME->UpdateBase(config.MIME_Path);
    config.Locations.SetServer(weak_from_this());
    try { // if acceptor=>Port == config.Port
        acceptor = ip::tcp::acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), config.Port));
    } catch(...) {}
}

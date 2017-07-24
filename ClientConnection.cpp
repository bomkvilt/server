
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

#include "ClientConnection.h"
#include "Message/Message.h"
#include "Service.h"
#include "Server.h"

using namespace srv::Conennection;

ClientStates::ClientStates(io_service &service) :
    bStarted(false),
    Timer(service),
    Timeout(7000)
{}

ClientConnection::ClientConnection(
        io_service& service,
        ServerConfig& Config
) :
    socket(service),
    states(service),
    Config(Config),
    ReadBuffer(),
    WriteBiffer()
{}

ClientConnection::ptr ClientConnection::Create(
        io_service& service,
        ServerConfig& Config
) {
    return ptr(new ClientConnection(service, Config));
}

/****************************************|  |****************************************/

void ClientConnection::Start() {
    states.bStarted = 1;
    UpdateLastPing();

    do_read();
}

void ClientConnection::Stop() {
    check(states.bStarted);
    states.bStarted = 0;
    states.Timer.cancel();
    socket.close();
    if (on_stop)
        on_stop();
}

/****************************************|  |****************************************/

ClientStates& ClientConnection::GetStates() {
    return states;
}

ip::tcp::socket& ClientConnection::Socket() {
    return socket;
}

/****************************************|  |****************************************/

void ClientConnection::UpdateLastPing() {
    states.LastPing = boost::posix_time::microsec_clock::local_time();
}

void ClientConnection::ResolveParametrs(message::Message& Message) {
    if (Message.GetDirective("Connection") == "close")
        Stop();
}

void ClientConnection::on_error(const ErrorCode& err) {
    check(states.bStarted);
    std::cout << " -- Connection Error: " << err.message() << std::endl;
    Stop();
}

/****************************************|  |****************************************/

void ClientConnection::do_read() {
    check(states.bStarted);
    async_read_until(
            socket,
            ReadBuffer,
            '\n',
            MEM_FC2(on_read,_1,_2)
    );
    do_ping();
}

void ClientConnection::do_write() {
    check(states.bStarted);
    async_write(
            socket,
            WriteBiffer,
            MEM_FC2(on_write,_1,_2)
    );
}

void ClientConnection::do_ping() {
    states.Timer.expires_from_now(states.Timeout);
    states.Timer.async_wait(MEM_FN(on_ping_check));
}

/****************************************|  |****************************************/

void ClientConnection::on_read(const ErrorCode& err, size_t bytes) {
    check(!err) on_error(err);      //TODO: check try again

    message::Message taken   = (ReadBuffer);
    message::Message reponse = Config.Locations.ResolveRequest(taken);
    WriteBiffer << reponse;
    do_write();
    ResolveParametrs(taken);
}

void ClientConnection::on_write(const ClientConnection::ErrorCode &err, size_t bytes) {
    check(!err) on_error(err);      //TODO: check try again
    do_read();
}

void ClientConnection::on_ping_check() {
    auto now = Time::microsec_clock::local_time();
    if ((now - states.LastPing) > states.Timeout) {
        Stop();
    } states.LastPing = now;
}

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

service::ClientStates::ClientStates(io_service &service) :
    bStarted(false),
    Timer(service),
    Timeout(200000)
{}

AClientConnection::AClientConnection(
        io_service& service,
        ServerConfig& Config
) :
    socket(service),
    states(service),
    Config(Config),
    ReadBuffer(),
    WriteBiffer()
{}

AClientConnection::ptr AClientConnection::Create(
        io_service& service,
        ServerConfig& Config
) {
    return ptr(new AClientConnection(service, Config));
}

/****************************************|  |****************************************/

void AClientConnection::Start() {
    states.bStarted = 1;
    UpdateLastPing();

    do_read();
}

void AClientConnection::Stop() {
    check(states.bStarted);
    states.bStarted = 0;
    states.Timer.cancel();
    socket.close();
    if (on_stop)
        on_stop();
}

/****************************************|  |****************************************/

service::ClientStates& AClientConnection::GetStates() {
    return states;
}

ip::tcp::socket& AClientConnection::Socket() {
    return socket;
}

/****************************************|  |****************************************/

void AClientConnection::UpdateLastPing() {
    states.LastPing = boost::posix_time::microsec_clock::local_time();
}

void AClientConnection::ResolveParametrs(message::AMessage& Message) {
    if (Message.GetDirective("Connection") == "close")
        Stop();
}

void AClientConnection::on_error(const ErrorCode& err, service::EActionType Action) {
    using namespace service;
    SWITCHM(err) {
        CASERM(error::eof)          Stop();
        CASEM (error::try_again)
            SWITCH(Action) {
                CASER(CCAT_READ)    do_read();
                CASER(CCAT_WRITE)   do_write();
            }
    }}
    Config.ErrorLog << " -- Connection Error: "
                    << err.message()
                    << std::endl;
    Stop();
}

/****************************************|  |****************************************/

void AClientConnection::do_read() {
    check(states.bStarted);
    async_read_until(
            socket,
            ReadBuffer,
            '\n',
            MEM_FC2(on_read,_1,_2)
    );
    do_ping();
}

void AClientConnection::do_write() {
    check(states.bStarted);
    async_write(
            socket,
            WriteBiffer,
            MEM_FC2(on_write,_1,_2)
    );
}

void AClientConnection::do_ping() {
    states.Timer.expires_from_now(states.Timeout);
    states.Timer.async_wait(MEM_FN(on_ping_check));
}

/****************************************|  |****************************************/

void AClientConnection::on_read(const ErrorCode& err, size_t bytes) {
    check(!err) on_error(err, service::CCAT_WRITE);

    message::AMessage taken   = (ReadBuffer);
    message::AMessage reponse = Config.Locations.ResolveRequest(taken);
    WriteBiffer << reponse;
    do_write();
    ResolveParametrs(taken);
}

void AClientConnection::on_write(const AClientConnection::ErrorCode &err, size_t bytes) {
    check(!err) on_error(err, service::CCAT_WRITE);
    do_read();
}

void AClientConnection::on_ping_check() {
    auto now = time::microsec_clock::local_time();
    if ((now - states.LastPing) > states.Timeout) {
        Stop();
    } states.LastPing = now;
}
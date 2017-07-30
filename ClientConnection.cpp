
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include <boost/lexical_cast.hpp>
//#include <boost/uuid/>

#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "Service.h"
#include "Server.h"

using namespace srv::conennection;



AClientConnection::AClientConnection(
        io_service& service,
        ServerConfig& Config
)   : socket(service)
    , states(service)
    , Config(Config)
{
    ALocation SignUp(this);
    SignUp.ExprType     = LEXT_MATCH;
    SignUp.ResultType   = LORT_APP;
    SignUp.Expr         = "/signup";
    SignUp.AppBack      = MEM_FT1(on_registration, _1);

    ALocation SignIn(this);
    SignIn.ExprType     = LEXT_MATCH;
    SignIn.ResultType   = LORT_APP;
    SignIn.Expr         = "/login";
    SignIn.AppBack      = MEM_FT1(on_login, _1);


    Config.Locations
            .SetLocation(SignUp)
            .SetLocation(SignIn);
}

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
    checkR(states.bStarted);
    states.bStarted = 0;
    states.Timer.cancel();
    socket.close();
    if (on_stop)
        on_stop();
}

/****************************************|  |****************************************/

service::ClientStates& AClientConnection::GetStates()
{ return states; }

ip::tcp::socket& AClientConnection::Socket()
{ return socket; }

/****************************************|  |****************************************/

void AClientConnection::UpdateLastPing() {
    states.LastPing = time::microsec_clock::local_time();
}

void AClientConnection::ResolveParamethrs(AMessage& Message) {
    if (Message["Connection"] == "close")
        Stop();
}

/****************************************|  |****************************************/

void AClientConnection::do_read()
{ do_read(MEM_FT2(on_read,  _1, _2)); }

void AClientConnection::do_write()
{ do_write(MEM_FT2(on_write, _1, _2)); }

void AClientConnection::do_read(internal_clb OnRead) {
    checkR(states.bStarted);
    async_read_until(
            socket,
            ReadBuffer,
            '\n',
            OnRead
    );
    do_ping();
}

void AClientConnection::do_write(internal_clb OnWrite) {
    checkR(states.bStarted);
    async_write(
            socket,
            WriteBiffer,
            OnWrite
    );
}

void AClientConnection::do_write_read(internal_clb OnRead) {
    checkR(states.bStarted);

    auto DoRead =
    [&OnRead, this]()
    { do_read(OnRead); };

    auto OnWrite =
    [&DoRead, this](const auto& err, auto bytes) {
        checkR(!err) this->on_error(err, service::CCAT_WRITE);
        DoRead();
    };

    do_write(OnWrite);
}

void AClientConnection::do_ping() {
    states.Timer.expires_from_now(states.Timeout);
    states.Timer.async_wait(MEM_FS(on_ping_check));
}

/****************************************|  |****************************************/

void AClientConnection::on_read(const ErrorCode& err, size_t bytes) {
    checkR(states.bAuthorized) on_error(401);
    checkR(!err) on_error(err, service::CCAT_READ);

    message::AMessage taken   = (ReadBuffer);
    message::AMessage reponse = Config.Locations.ResolveRequest(taken, this);
    WriteBiffer << reponse;
    do_write();
    ResolveParamethrs(taken);
}

void AClientConnection::on_write(const ErrorCode &err, size_t bytes) {
    checkR(!err) on_error(err, service::CCAT_WRITE);
    do_read();
}

void AClientConnection::on_ping_check() {
    auto now = time::microsec_clock::local_time();
    if ((now - states.LastPing) > states.Timeout) {
        return Stop();
    } states.LastPing = now;
}

/****************************************|  |****************************************/

void AClientConnection::on_error(const ErrorCode& err, service::EActionType Action) {
    using namespace service;
    SWITCHM(err) {
    CASERM(error::eof       )   Stop();
    CASEM (error::try_again )
        SWITCH(Action) {
        CASER(CCAT_READ )       do_read();
        CASER(CCAT_WRITE)       do_write();
        }

    }}

    on_error(500);
}

void AClientConnection::on_error(int code, const std::string Body) {
    AMessage Message(std::to_string(code), "Err", true);
    Message.Body = Body;
    WriteBiffer << Message;
    do_write();
    Stop();
}

AMessage AClientConnection::on_login_error(std::string Body) {
    AMessage Message(std::to_string(500), "Err", true);
    Message.Body = Body;

    Config.ErrorLog << " -- Login Error: " << Body << std::endl;

    return Message;
}

AMessage AClientConnection::on_signUp_error() {
    //
}

/**************************************|  |**************************************/

AMessage AClientConnection::on_registration(const AMessage& Message) {
    std::string key = Message.Body;
    int guid        = generate_guid();

    on_new_user(guid, key);                     //TODO::failed creation
    states.bAuthorized = true;

    auto Reponse = SuccessMessage(true);
    Reponse.Body = std::to_string(guid);

    Config.SuccessLog << " -- New user created. "
                      << "Guid: " << guid
                      << std::endl;

    return Reponse;
}

AMessage AClientConnection::on_login(const AMessage& Message) {
    int guid = std::stoi(Message.Body);

    auto key = Config.Users[guid];
    checkR(key.length()) on_login_error();

    RSA* rsa = create_rsa(key);
    checkR(rsa) on_login_error();

    std::string enc;
    enc.reserve(RSA_size(rsa));
    std::string ran = generate_str(35, 80);

    int length = RSA_public_encrypt(
            (int)key.length() + 1
            , (unsigned char*)ran.c_str()
            , (unsigned char*)enc.c_str()
            , rsa
            , RSA_PKCS1_OAEP_PADDING
    ); RSA_free(rsa);
    checkR(length == -1) on_login_error();

    AMessage Reponse = SuccessMessage(false);
    Reponse.Body = enc;
    return Reponse;
}

AMessage AClientConnection::on_login_end(const AMessage& Message, std::string check_str) {
    std::string ret = Message.Body;

    checkR(ret == check_str) on_login_error();

    states.bAuthorized = true;
    AMessage Reponse("200", "OK", false);
    return Reponse;
}

int AClientConnection::generate_guid()
{ return 1566; }

std::string AClientConnection::generate_str(int min, int max) {
    int leght = min + std::rand() % (max - min + 1);

    std::string tmp;
    tmp.resize(leght);
    for (int i = 0; i < leght; ++i)
        tmp[i] = 32 + rand() % (95);
    return tmp;
}

RSA* AClientConnection::create_rsa(const std::string& key) {
    BIO* bio = BIO_new_mem_buf(key.c_str(), (int)key.size());
    checkR(bio) nullptr;

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    check(pkey) {
        BIO_free(bio);
        return nullptr;
    }

    RSA* rsa = EVP_PKEY_get1_RSA(pkey);
    EVP_PKEY_free(pkey);
    BIO_free(bio);
    return  rsa;
}


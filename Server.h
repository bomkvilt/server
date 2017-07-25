#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "Service.h"
#include "MIME/MIME_Detector.h"
#include "Location/LocationHandler.h"
#include "ClientConnection.h"

namespace srv {
    using namespace Conennection;

    /**
     *
     */
    struct ServerConfig {
        bool bStarted;
        //bool bAccept;

        unsigned short            Port;
        std::string               WorkPath;
        std::string               MIME_Path;
        std::ostream&             AccessLog;
        std::ostream&             ErrorLog;
        MIME_Detector::ptr        MIME;
        location::LocationHandler Locations;

        //size_t MaxConnections;

    public:
        ServerConfig();
    };


    /**
     *
     */
    class AServer :
            public srv::enable_weak_from_this<AServer>,
            boost::noncopyable
    {
    public:
        DEFINE_SELF(AServer);
        DEFINE_PRS(AServer);

        typedef AClientConnection::ptr  client_ptr;
        typedef AClientConnection::wptr client_wptr;

    protected:
        typedef boost::system::error_code ErrorCode;

    protected:  /************************| Construction |************************/
        AServer();

    public:
        static ptr Create();
        ~AServer();

    public:     /************************| Control |************************/
        void Start();
        void Stop();

        void Refresh();

        void RemoveClients();

    protected:  /************************| Members |************************/
        ServerConfig      config;
        io_service        service;
        ip::tcp::acceptor acceptor;

        std::vector<client_ptr> clients;

    public:
        //***| Getters && Setters |
        ServerConfig &GetConfig();

        size_t ClientCount() const;

    protected:  /************************|  |************************/
        client_wptr NewClient();

        void HandleAccept(client_wptr Client, const AServer::ErrorCode &Err);

        void HandleUnbind(client_wptr Client);

        void do_accept();

        void update_dependencies();
    };


}
#endif //SERVER_SERVER_H

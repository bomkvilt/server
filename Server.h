#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <map>
#include <list>

#include "Service.h"
#include "MIME/MIME_Detector.h"
#include "Location/LocationHandler.h"
#include "ClientConnection.h"

namespace srv {
    using namespace conennection;
    using namespace message;


    /**
     *
     */
    struct ServerConfig {
        typedef std::map<int, const std::string> LUsers;
        typedef std::list<int>                   LServers;
    public:
        bool bStarted;
        //bool bAccept;

        unsigned short Port;
        std::string         WorkPath;
        std::string         MIME_Path;

        std::ostream&       SuccessLog;
        std::ostream&       ErrorLog;

        MIME_Detector::ptr  MIME;
        LocationHandler     Locations;

        LUsers              Users;
        LServers            Servers;

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


        typedef std::vector<client_ptr>     LClients;

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

        LClients clients;

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

    protected:  /************************|  |************************/

        void HandleNewUser(int guid, const std::string& key);

        void add_user_to_index(int guid);

        void create_user_derectory(int guid);

    };


}
#endif //SERVER_SERVER_H

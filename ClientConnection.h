#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <chrono>
#include <functional>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "Service.h"
#include "MIME/MIME_Detector.h"
#include "Message/Message.h"

namespace srv {
//*/
    struct ServerConfig;
//*/
namespace Conennection {
    using namespace boost::asio;
    namespace asio = boost::asio;
    namespace Time = boost::posix_time;

    /**
     *
     */
    struct ClientStates {
        bool bStarted;

        deadline_timer Timer;
        Time::ptime    LastPing;
        Time::millisec Timeout;

    public:
        ClientStates(io_service &service);
    };


    /**
     *
     */
    class ClientConnection :
            public srv::enable_weak_from_this<ClientConnection>,
            boost::noncopyable {

    public:
        DEFINE_SELF(ClientConnection);
        DEFINE_PRS(ClientConnection);

        typedef boost::system::error_code ErrorCode;
        typedef boost::function<void()> on_stop_clb;

    protected:  /************************|  |************************/
        //***| Construction |
        ClientConnection(
                io_service &service,
                ServerConfig& Config
        );

    public:
        static ptr Create(
                io_service &service,
                ServerConfig& Config
        );

        template<typename Callback>
        static ptr Create(
                io_service &service,
                ServerConfig& Config,
                Callback clb
        ) {
            auto tmp = Create(service, Config);
            tmp->SetStopCallback(clb);
            return tmp;
        }

    public:     /************************| Control |************************/
        void Start();

        void Stop();

    protected:  /************************| Members |************************/
        //***| members |
        ip::tcp::socket socket;
        ClientStates    states;
        ServerConfig&   Config;

        asio::streambuf ReadBuffer;
        asio::streambuf WriteBiffer;

        on_stop_clb on_stop;
    public:
        //***| getters && setters |
        ClientStates &GetStates();

        ip::tcp::socket &Socket();

        template<typename Callback>
        void SetStopCallback(Callback clb) {
            on_stop = MEM_FF1(clb, weak_from_this());
        }

    protected:  /************************|  |************************/
        void UpdateLastPing();

        void ResolveParametrs(message::Message& Message);

        void on_error(const ErrorCode &err);

        void do_read();

        void do_write();

        void do_ping();

        void on_read(const ErrorCode &err, size_t bytes);

        void on_write(const ErrorCode &err, size_t bytes);

        void on_ping_check();
    };
}
}

#endif //SERVER_CLIENT_H

#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "Service.h"
#include "Location/LocationHandler.h"
#include "MIME/MIME_Detector.h"
#include "Message/Message.h"


namespace srv {
//*/
    struct ServerConfig;
//*/
namespace conennection {
    using namespace boost::asio;
    using namespace message;
    using namespace location;

    namespace asio  = boost::asio;
    namespace time  = boost::posix_time;
    namespace error = asio::error;

    namespace service {

        enum EActionType {
            CCAT_READ,
            CCAT_WRITE
        };

        /**
         *
         */
        struct ClientStates {
            bool bStarted;
            bool bAuthorized;

            deadline_timer Timer;
            //TODO::ping timer
            time::ptime LastPing;
            time::millisec Timeout;
            time::millisec PingTime;

        public:
            ClientStates(io_service &service) :
                bStarted(false)
                , bAuthorized(false)
                , Timer(service)
                , Timeout(20000)
                , PingTime(5000)
            {}
        };
    }

    /**
     *
     */
    class AClientConnection :
            public srv::enable_weak_from_this<AClientConnection>,
            boost::noncopyable
    {
    public:
        DEFINE_SELF(AClientConnection);
        DEFINE_PRS(AClientConnection);


        typedef boost::system::error_code ErrorCode;
        typedef boost::function<void(const ErrorCode& err, size_t bytes)> internal_clb;
        typedef boost::function<void(int, const std::string&)>            registration_clb;
        typedef boost::function<void()>                                   stop_clb;

    protected:  /************************| Construction |************************/

        AClientConnection(io_service &service, ServerConfig& Config);
    public:
        static ptr Create(io_service &service, ServerConfig& Config);

    public:     /************************| Control |************************/

        void Start();
        void Stop();

    protected:  /************************| Members |************************/

        ip::tcp::socket         socket;
        service::ClientStates   states;
        ServerConfig&           Config;

        asio::streambuf         ReadBuffer;
        asio::streambuf         WriteBiffer;

        registration_clb        on_new_user;
        stop_clb                on_stop;

    public:

        service::ClientStates&  GetStates();
        ip::tcp::socket&        Socket();

        template<typename Callback>
        ptr SetStopCallback(Callback clb) {
            on_stop = MEM_FN1(clb, weak_from_this());
            return shared_from_this();
        }

        template<typename Callback>
        ptr SetNewUserCallback(Callback clb) {
            on_new_user = MEM_FN2(clb,_1, _2);
            return shared_from_this();
        }

    protected:  /************************|  |************************/
        void UpdateLastPing();

        void ResolveParamethrs(AMessage& Message);

        void do_read();
        void do_write();

        void do_read (internal_clb OnRead);
        void do_write(internal_clb OnWrite);

        void do_write_read(internal_clb OnRead);

        void do_ping();


        void on_read (const ErrorCode &err, size_t bytes);
        void on_write(const ErrorCode &err, size_t bytes);
        void on_error(const ErrorCode& err, service::EActionType Action);
        void on_error(int code, std::string Body = "");

        AMessage on_login_error(std::string Body = "");
        AMessage on_signUp_error();

        void on_ping_check();

    protected:  /************************|  |************************/

        AMessage on_registration(const AMessage& Message);
        AMessage on_login       (const AMessage& Message);
        AMessage on_login_end   (const AMessage& Message, std::string check_str);

        int         generate_guid();
        std::string generate_str(int min, int max);
        RSA*        create_rsa(const std::string& key);
    };
}
}

#endif //SERVER_CLIENT_H

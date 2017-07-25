#ifndef SERVER_LOCATION_H
#define SERVER_LOCATION_H

#include <string>
#include <boost/function.hpp>
#include "../Message/Message.h"
#include "../MIME/MIME_Detector.h"

namespace srv {
/**/class AServer;
namespace location {
/**/class LocationHandler;

    enum EExprType {
        LEXT_MATCH,
        LEXT_PREFIX_FORCE,
        LEXT_REGULAR,
        LEXT_PREFIX
    };

    enum EResultType {
        LORT_ERROR,
        LORT_FILE,
        LORT_APP,
        LORT_CONTROLL
    };

    /**
     *
     */
    class ALocation
    {
        friend class LocationHandler;
    public:
        typedef boost::function<message::AMessage(const message::AMessage&             )> AppBack_clb;
        typedef boost::function<message::AMessage(const message::AMessage&, PTR(AServer))> Control_clb;

    public:
        EExprType    ExprType;
        EResultType  ResultType;
        std::string  Expr;         // path, prefix or regular expression
        std::string  Root;         // path to fails relative server root(@BaseRoot)
        AppBack_clb  AppBack;      // application callback
        Control_clb  ControlBack;  // callback for server control
    protected:
        WPTR(AServer) server;

    public:
        message::AMessage ProcessMessage(const message::AMessage &Message);

    protected:
        PTR(AServer)       Server();
        PTR(MIME_Detector) MIME();
        std::string        BaseRoot();

        std::string GetClearPath(std::string Path);

        message::AMessage on_error  (const message::AMessage& Message);
        message::AMessage on_file   (const message::AMessage& Message);
        message::AMessage on_file_r (const message::AMessage& Message, const std::string& Path);
        message::AMessage on_file_w (const message::AMessage& Message, const std::string& Path);
        message::AMessage on_app    (const message::AMessage& Message);
        message::AMessage on_control(const message::AMessage& Message);
    };
}
}

#endif //SERVER_LOCATION_H

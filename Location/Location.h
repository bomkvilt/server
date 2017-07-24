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
    class Location
    {
        friend class LocationHandler;
    public:
        typedef boost::function<message::Message(const message::Message&             )> AppBack_clb;
        typedef boost::function<message::Message(const message::Message&, PTR(AServer))> Control_clb;

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
        message::Message ProcessMessage(const message::Message &Message);

    protected:
        PTR(AServer)       Server();
        PTR(MIME_Detector) MIME();
        std::string        BaseRoot();

        std::string GetClearPath(std::string Path);

        message::Message on_error  (const message::Message& Message);
        message::Message on_file   (const message::Message& Message);
        message::Message on_app    (const message::Message& Message);
        message::Message on_control(const message::Message& Message);
    };
}
}

#endif //SERVER_LOCATION_H

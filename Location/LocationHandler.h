#ifndef SERVER_LOCATIONHANDLER_H
#define SERVER_LOCATIONHANDLER_H

#include <vector>
#include "../MIME/MIME_Detector.h"
#include "Location.h"

namespace srv {
/**/class AServer;
namespace location {

    /**
     *
     */
    class LocationHandler
    {
    public:
        typedef std::vector<ALocation> FLocations;

    public:
        LocationHandler();

        message::AMessage ResolveRequest(const message::AMessage &Message);

        FLocations& GetLocations();
        LocationHandler& SetLocations(FLocations& l);
        LocationHandler& SetLocation(ALocation& l);

    protected:
        ALocation     DefaultLocation;
        FLocations   Locations;
        WPTR(AServer) server;

    public:
        LocationHandler& SetServer(WPTR(AServer) server);

    protected:
        size_t CalculateRelevant(const ALocation& l, const std::string& Url);
        size_t CalculateMatch   (const ALocation& l, const std::string& Url);
        size_t CalculatePrefix  (const ALocation& l,       std::string  Url);
        size_t CalculateRegexp  (const ALocation& l, const std::string& Url);
    };

}
}

#endif //SERVER_LOCATIONHANDLER_H

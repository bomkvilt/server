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
        typedef std::vector<Location> FLocations;

    public:
        LocationHandler();

        message::Message ResolveRequest(const message::Message &Message);

        FLocations& GetLocations();
        LocationHandler& SetLocations(FLocations& l);
        LocationHandler& SetLocation(Location& l);

    protected:
        Location     DefaultLocation;
        FLocations   Locations;
        WPTR(AServer) server;

    public:
        LocationHandler& SetServer(WPTR(AServer) server);

    protected:
        size_t CalculateRelevant(const Location& l, const std::string& Url);
        size_t CalculateMatch   (const Location& l, const std::string& Url);
        size_t CalculatePrefix  (const Location& l,       std::string  Url);
        size_t CalculateRegexp  (const Location& l, const std::string& Url);
    };

}
}

#endif //SERVER_LOCATIONHANDLER_H


#include <regex>
#include "LocationHandler.h"

using namespace srv::location;
using namespace srv;

LocationHandler::LocationHandler() {
    DefaultLocation.ResultType = LORT_ERROR;
}

message::AMessage LocationHandler::ResolveRequest(const message::AMessage &Message) {
    size_t max = 0;
    auto& Loc  = DefaultLocation;
    auto& Url  = Message.Path;
    for (auto& i : Locations) {
        size_t tmp = CalculateRelevant(i, Url);
        if (tmp && tmp >= max) {
            max = tmp;
            Loc = i;
    }}
    return Loc.ProcessMessage(Message);
}

LocationHandler::FLocations &LocationHandler::GetLocations() {
    return Locations;
}

LocationHandler& LocationHandler::SetLocations(LocationHandler::FLocations &l) {
    Locations.clear();
    for (auto& i : l)
        SetLocation(i);
    return *this;
}

LocationHandler& LocationHandler::SetLocation(ALocation& l) {
    Locations.push_back(l);
    Locations.back().server = server;
    return *this;
}

LocationHandler& LocationHandler::SetServer(WPTR(AServer) server) {
    this->server = server;
    for (auto& i : Locations) {
        i.server = server;
    } return *this;
}

size_t LocationHandler::CalculateRelevant(const ALocation &l, const std::string &Url) {
    const size_t match    = 10000;
    const size_t prefix_f = 200;
    const size_t regex    = 100;
    const size_t prefix   = 1;

    switch (l.ExprType) {
        CASER(LEXT_MATCH)           CalculateMatch (l,Url) * match;
        CASER(LEXT_PREFIX_FORCE)    CalculatePrefix(l,Url) * prefix_f;
        CASER(LEXT_REGULAR)         CalculateRegexp(l,Url) * regex;
        CASER(LEXT_PREFIX)          CalculatePrefix(l,Url) * prefix;
    } return 0;
}

size_t LocationHandler::CalculateMatch(const ALocation &l, const std::string &Url) {
    return Url == l.Expr ? 1 : 0;
}

#define NORMALISE(STR) if (STR.back() != '/') STR += '/'

size_t LocationHandler::CalculatePrefix(const ALocation &l, std::string Url) {
    std::string Exp = l.Expr;
    NORMALISE(Url);
    NORMALISE(Exp);

    std::regex e("([^/^[:s:]]*/)");
    std::sregex_iterator Ei(Exp.begin(), Exp.end(), e);
    std::sregex_iterator Ui(Url.begin(), Url.end(), e);
    std::sregex_iterator end;

    size_t factor = 0;
    for (;Ei != end && Ui != end; ++factor)
        if (Ei++->str(1) != Ui++->str(1))
            break;
    return factor;
}

size_t LocationHandler::CalculateRegexp(const ALocation &l, const std::string &Url) {
    std::smatch m;
    std::regex  e(l.Expr);
    return std::regex_search(Url.begin(), Url.end(), m, e) ? 1 : 0;
}

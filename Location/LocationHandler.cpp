
#include <regex>
#include "LocationHandler.h"

using namespace srv::location;
using namespace srv;

LocationHandler::LocationHandler() {
    DefaultLocation.ResultType = LORT_ERROR;
}

message::Message LocationHandler::ResolvRequet(const message::Message& Message) {
    size_t max = 0;
    auto& Loc  = DefaultLocation;
    auto& Url  = Message.Path;
    for (auto& i : Locations) {
        size_t tmp = CalculateRelivant(i,Url);
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

LocationHandler& LocationHandler::SetLocation(Location& l) {
    Locations.push_back(l);
    Locations.back().MIME     = MIME;
    Locations.back().BaseRoot = Root;
    return *this;
}

LocationHandler& LocationHandler::SetMIME(MIME_Detector::ptr MIME) {
    this->MIME = MIME;
    for (auto& i : Locations)
        i.MIME = MIME;
    return *this;
}

LocationHandler& LocationHandler::SetRoot(std::string Root) {
    this->Root = Root;
    for (auto& i : Locations)
        i.BaseRoot = Root;
    return *this;
}

size_t LocationHandler::CalculateRelivant(const Location &l, const std::string& Url) {
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

size_t LocationHandler::CalculateMatch(const Location &l, const std::string &Url) {
    return Url == l.Expr ? 1 : 0;
}

size_t LocationHandler::CalculatePrefix(const Location &l, const std::string &Url) {
    std::regex e("([^/^[:s:]]*/)");
    std::sregex_iterator Ei(l.Expr.begin(), l.Expr.end(), e);
    std::sregex_iterator Ui(Url.begin(),   Url.end(),     e);
    std::sregex_iterator end;

    size_t factor = 0;
    for (;Ei != end && Ui != end; ++factor)
        if (Ei++->str(1) != Ui++->str(1))
            break;
    return factor;
}

size_t LocationHandler::CalculateRegexp(const Location &l, const std::string &Url) {
    std::smatch m;
    std::regex  e(l.Expr);
    return std::regex_search(Url.begin(), Url.end(), m, e) ? 1 : 0;
}


#include <fstream>
#include <algorithm>
#include <iterator>
#include <regex>
#include <iostream>
#include "../Service.h"
#include "MIME_Detector.h"

using namespace srv;

MIME_Detector::ptr MIME_Detector::Create() {
    return ptr(new MIME_Detector);
}

MIME_Detector::ptr MIME_Detector::Create(const std::string& Path) {
    ptr tmp = Create();
    tmp->UpdateBase(Path);
    return tmp;
}

#define Extantion "\.[[:w:][:d:]]+"

void MIME_Detector::UpdateBase(const std::string &Path) {
    std::ifstream in(Path);
    check(in.is_open());

    std::string tmp;
    in >> std::noskipws;
    std::copy(std::istream_iterator<char>(in),
              std::istream_iterator<char>(),
              std::back_inserter(tmp)
    ); tmp += "\r\n";

    std::regex e("(" Extantion ")\t\t([\-[:w:][:d:]]+/[\-[:w:][:d:]]+)\r?\n");
    std::sregex_iterator itr(tmp.cbegin(), tmp.cend(), e);
    std::sregex_iterator end;
    for (; itr != end; ++itr)
        MIME_table[itr->str(1)] = itr->str(2);
}

void MIME_Detector::ClearBase() {
    MIME_table.clear();
}

#define TAIL "@@"

std::string MIME_Detector::GetExtantion(std::string FileName) {
    FileName += TAIL;

    std::smatch m;
    std::regex e("(" Extantion ")" TAIL);
    if (std::regex_search(FileName, m, e))
        return m[1].str();
    return "";
}

std::string MIME_Detector::GetName(std::string FileName) {
    FileName += TAIL;

    std::smatch m;
    std::regex e("([^[:s:]^/]+)" TAIL);
    if (std::regex_search(FileName, m, e))
        return m[1].str();
    return "";
}

std::string MIME_Detector::GetMIME(const std::string& FileName) {
    std::string ext = GetExtantion(FileName);
    return MIME_table[ext];
}

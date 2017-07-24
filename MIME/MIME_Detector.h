#ifndef SERVER_IMEI_DETECTOR_H
#define SERVER_IMEI_DETECTOR_H

#include <map>
#include <string>
#include <memory>
#include "../Service.h"

#include <iostream>

namespace srv
{

    /**
     *
     */
    class MIME_Detector :
            public srv::enable_weak_from_this<MIME_Detector>
    {
    protected:
        typedef std::map<std::string, std::string> FMIME_table;

    public:
        DEFINE_SELF(MIME_Detector);
        DEFINE_PRS(MIME_Detector);

    protected:  /************************| Construction |************************/

        MIME_Detector() = default;
    public:
        static ptr Create();
        static ptr Create(const std::string& Path);

        void UpdateBase(const std::string& Path);
        void ClearBase();

    public:
        static std::string GetExtension(std::string FileName);
        static std::string GetName(std::string FileName);
        std::string GetMIME(const std::string& FileName);

    protected:
        FMIME_table MIME_table;
    };
}


#endif //SERVER_IMEI_DETECTOR_H

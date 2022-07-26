#ifndef PATHTRANSLATION_H
#define PATHTRANSLATION_H

#include "XrdOuc/XrdOucName2Name.hh"
#include "XrdSys/XrdSysError.hh"
#include <iostream>
#include <json/value.h>
#include <json/json.h>

namespace XrdCmsJson
{

# define XRDCMSJSON_OK 0
# define XRDCMSJSON_ERR_PARSERULE 1
# define XRDCMSJSON_ERR_URL 2
# define XRDCMSJSON_ERR_FILE 3
# define XRDCMSJSON_ERR_NOLFN2PFN 4

class PathTranslation : public XrdOucName2Name
{
public:
    PathTranslation (XrdSysError *lp, const char * url_translation);
    virtual ~PathTranslation (){};
    int lfn2pfn(const char *lfn, char *buff, int blen);
    int parse();


private:
    //std::string m_fileType;
    std::string m_filename;
    //std::string m_destination;
    std::string m_url;
    std::string m_volume;
    std::string m_protocol;
    std::string m_destination;    
    Json::Value	m_protocol_json;

    XrdSysError *eDest;
};

}

extern "C"
{
XrdOucName2Name *XrdOucgetName2Name(XrdOucgetName2NameArgs);
}

#endif

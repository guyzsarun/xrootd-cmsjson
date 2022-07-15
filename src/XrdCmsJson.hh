#ifndef PATHTRANSLATION_H
#define PATHTRANSLATION_H

#include "XrdSys/XrdSysError.hh"
#include <iostream>

namespace XrdCmsJson
{

# define XRDCMSJSON_OK 0
# define XRDCMSJSON_ERR_PARSERULE 1
# define XRDCMSJSON_ERR_URL 2
# define XRDCMSJSON_ERR_FILE 3
# define XRDCMSJSON_ERR_NOLFN2PFN 4

class PathTranslation 
{
public:
    PathTranslation (XrdSysError *lp, const char * tfc_file);
    virtual ~PathTranslation (){};
    int lfn2pfn(const char *lfn, char *buff, int blen);
    int parse();

private:
    //std::string m_fileType;
    std::string m_filename;
    //std::string m_destination;
    std::string m_url;
    std::string m_protocol;

    XrdSysError *eDest;
};

}
#endif

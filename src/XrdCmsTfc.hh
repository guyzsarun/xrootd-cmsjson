#ifndef TRIVIALFILECATALOG_H
#define TRIVIALFILECATALOG_H

#include "XrdSys/XrdSysError.hh"
#include <iostream>

namespace XrdCmsTfc
{

# define XRDCMSTFC_OK 0
# define XRDCMSTFC_ERR_PARSERULE 1
# define XRDCMSTFC_ERR_URL 2
# define XRDCMSTFC_ERR_FILE 3
# define XRDCMSTFC_ERR_NOLFN2PFN 4

class TrivialFileCatalog 
{
public:
    TrivialFileCatalog (XrdSysError *lp, const char * tfc_file);
    virtual ~TrivialFileCatalog (){};
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

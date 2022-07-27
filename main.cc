#include <iostream>
#include <sys/param.h>
#include "src/XrdCmsJson.hh"
#include "XrdSys/XrdSysError.hh"
#include "XrdSys/XrdSysLogger.hh"


int main (int, const char** argv)
{
   //const char* lfn = "/store/test/14c5c58e-00c2-4660-bf90-a963b86388e1.root";
   int blen = 4096;
   char* buff = (char*) malloc(blen);
   char rf[] = "file:/root/xrootd-cmsjson/storage_test.json?volume=Test_dCache&protocol=WebDAV";
   XrdSysLogger myLogger;
   XrdSysError eDest(&myLogger, "tfc_");
   eDest.Say("TFC Module");
   XrdCmsJson::PathTranslation *cmsJson = new XrdCmsJson::PathTranslation (&eDest, rf);
   const char* lfn = argv[1];
   cmsJson->lfn2pfn(lfn, buff, blen);
   std::cout << buff << std::endl;
   return 0;
}


#include <iostream>
#include <sys/param.h>
#include "src/XrdCmsJson.hh"
#include "XrdSys/XrdSysError.hh"
#include "XrdSys/XrdSysLogger.hh"


int main (int, const char** argv)
{
   const char* lfn = argv[1];
   const char *rf = argv[2];

   int blen = 4096;
   char* buff = (char*) malloc(blen);
   XrdSysLogger myLogger;
   XrdSysError eDest(&myLogger, "tfc_");
   eDest.Say("TFC Module");
   XrdCmsJson::PathTranslation *cmsJson = new XrdCmsJson::PathTranslation (&eDest, rf);
   cmsJson->lfn2pfn(lfn, buff, blen);
   return 0;
}

#include <iostream>
#include <sys/param.h>
#include "src/XrdCmsJson.hh"
#include "XrdSys/XrdSysError.hh"
#include "XrdSys/XrdSysLogger.hh"


int main (int, const char* argv)
{
   //const char* lfn = "/store/test/14c5c58e-00c2-4660-bf90-a963b86388e1.root";
   int blen = 4096;
   char* buff = (char*) malloc(blen);
   char rf[] = "~/xrootd-cmsjson/storage_test.json?volume=Test_dCache&protocol=WebDAV";
   XrdSysLogger myLogger;
   XrdSysError eDest(&myLogger, "tfc_");
   eDest.Say("TFC Module");
   XrdCmsTfc::TrivialFileCatalog tfc(&eDest, rf);
   tfc.lfn2pfn(argv[1], buff, blen);
   std::cout << buff << std::endl;
   return 0;
}

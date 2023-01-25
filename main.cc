#include <iostream>
#include <sys/param.h>
#include "src/XrdCmsJson.hh"
#include "XrdSys/XrdSysError.hh"
#include "XrdSys/XrdSysLogger.hh"


int main (int, const char** argv)
{
   //const char* lfn = "/store/test/xrootd/T2_XX_Test/test/store/14c5c58e-00c2-4660-bf90-a963b86388e1.root";
   //const char* lfn = "/store/data/Run2022C/EGamma/MINIAOD/PromptReco-v1/000/357/329/00000/3ca6789e-1a2e-4b96-82c3-6554522d88e6.root";
   const char* lfn = argv[1];

   //char rf[] = "file:/root/xrootd-cmsjson/storage_test.json?volume=Test_dCache&protocol=srm-chain";
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

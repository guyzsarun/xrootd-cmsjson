#ifndef PATHTRANSLATION_H
#define PATHTRANSLATION_H

#include "XrdOuc/XrdOucName2Name.hh"
#include "XrdSys/XrdSysError.hh"
#include <iostream>
#include <json/value.h>
#include <json/json.h>

namespace XrdCmsJson
{
/**
 *        @class PathTranslation
 *          	This class is the concrete implementation of the
 *          	CMS Path Translation as implemented by 4Quarks
 *              The adoptation was then done for Xrd to be a Name2Name module.
 *       @Author: 4Quarks
 */

#define XRDCMSJSON_OK 0
#define XRDCMSJSON_ERR_DATAMISSING 1 //info is missing
#define XRDCMSJSON_ERR_FORMAT 2 // info is incorrect
#define XRDCMSJSON_ERR_URL 3
#define XRDCMSJSON_ERR_JSON 4
#define XRDCMSJSON_ERR_PROTOCOL 5
#define XRDCMSJSON_ERR_RULE 6


class PathTranslation : public XrdOucName2Name
{
public:
    PathTranslation (XrdSysError *lp, const char * url_translation);

    virtual ~PathTranslation (){};

    int testCMSNamespaces();

    int lfn2pfn(const char *lfn, char *buff, int blen);

    int pfn2lfn(const char *pfn, char *buff, int blen);

    int lfn2rfn(const char *lfn, char *buff, int blen);

    int appendRuleJson(std::string lfn, std::string pfn, Json::Value *all_rules);

    std::string resolveChain(Json::Value rule_chain, Json::Value rule_defined);

    Json::Value getStorageJson();

    int reformatJson(Json::Value original_storage);

    int parse();

    int parseUrl();

    int parseStorageJson();

    int verifyFormatJson();

    int verifyFormatURL();

    int verifyFormatProtocol(Json::Value prot);

    Json::Value simplifyProtocol(Json::Value prot);

    Json::Value parseProtocol(Json::Value protocol, std::string lfn);

    std::string matchLFN(Json::Value rule, std::string target_lfn);

    int verifyFormatRule(Json::Value rule);

    Json ::Value parsePrefix(std::string pfn);

    Json::Value parseChain(Json::Value rule_root, Json::Value rule_chain);

    Json::Value getMatchRule(Json::Value rule_root);

    Json::Value buildRule (std::string lfn, std::string pfn, std::string chain);

private:

    const std::string CMS_NAMESPACE = "/store/.*";
    const std::string CMS_ALL_NAMESPACES[24] = {
        "/store/data/",
        "/store/hidata/",
        "/store/mc/",
        "/store/himc/",
        "/store/relval/",
        "/store/hirelval/",
        "/store/express/",
        "/store/results/",
        "/store/unmerged/",
        "/store/local/"
        "/store/backfill/1/",
        "/store/backfill/2/",
        "/store/generator/",
        "/store/mc/SAM/",
        "/store/mc/HC/",
        "/store/temp/",
        "/store/temp/user/",
        "/store/test/",
        "/store/user/",
        "/store/user/rucio/",
        "/store/group/",
        "/store/group/rucio/",
        "/store/test/xrootd/",
        "/store/test/loadtest/"
    };

    std::string m_filename;
    std::string m_url;
    std::string m_volume;
    std::string m_protocol;
    std::string m_destination;
    Json::Value	m_rules;
    Json::Value	m_json;

    XrdSysError *eDest;
};

}

extern "C"
{
XrdOucName2Name *XrdOucgetName2Name(XrdOucgetName2NameArgs);
}

#endif

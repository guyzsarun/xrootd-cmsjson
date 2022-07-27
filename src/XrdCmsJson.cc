#include "XrdCmsJson.hh"
#include "XrdSys/XrdSysError.hh"

#include <iostream>
#include <json/value.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <list>
#include <regex>

#define BUFFSIZE 1024
#define OVECCOUNT 30

using namespace XrdCmsJson;

extern "C"
{
    XrdOucName2Name *XrdOucgetName2Name(XrdSysError *eDest, const char *confg,
        const char *parms, const char *lroot, const char *rroot)
    {
        eDest->Say("CERN CMS Facilities and Services Site Support"); 

        eDest->Say("Params: ", parms);
        PathTranslation *myTranslation = new PathTranslation(eDest, parms);
        return myTranslation;
    }
}

XrdCmsJson::PathTranslation::PathTranslation(XrdSysError *lp, const char * url_translation) : XrdOucName2Name(), m_destination("any")
{
    m_url = url_translation;
    eDest = lp;
    parse();
}

int XrdCmsJson::PathTranslation::appendRuleJson (std::string lfn, std::string pfn, Json::Value *all_rules)
{
    Json::Value rule;
    rule["lfn"] = lfn;
    rule["pfn"] = pfn;
    all_rules->append(rule);
    return 0;
}

std::string XrdCmsJson::PathTranslation::resolveChain(Json::Value rule_chain, Json::Value rule_defined)
{
    std::string pfn = rule_defined["pfn"].asString();
    std::regex lfn_expr (rule_defined["lfn"].asString());
    std::string pfn_resolved = regex_replace(rule_chain["pfn"].asString(), lfn_expr, pfn);
    return pfn_resolved;
}


int XrdCmsJson::PathTranslation::parse ()
{
    eDest->Say("Conecting to the catalog ", m_url.c_str());
    m_filename = m_url.substr(0, m_url.find("?"));
    m_protocol = m_url.substr(m_url.find("protocol=")+9 ,m_url.length()); 
    m_volume = m_url.substr(m_url.find("volume=")+7 ,m_url.find("&protocol=")-(m_url.find("volume=")+7) ); 
    
    std::cout << "Volume " << m_volume << ", protocol " << m_protocol << std::endl;

    // First checks
    if (m_filename.empty() or m_protocol.empty()) 
    {
        eDest->Say("PathTranslation::connect: Malformed url for file catalog configuration");
        return XRDCMSJSON_ERR_URL;   
    }

    // Load json
    std::ifstream file("/root/xrootd-cmsjson/storage_test.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse( file, actualJson);

    if (actualJson.empty())
    {
        eDest->Say("The storage.json is empty or the path is incorrect. Information has not ben loaded");
        return XRDCMSJSON_ERR_URL; 
    }

    // Find desired protocol
    for(const auto& prot : actualJson[0]["protocols"])
    {
        if (prot["protocol"] == m_protocol) 
        {
            // Info type access 
            std::cout << "Protocol " << m_protocol << " with access: " << prot["access"].asString() << std::endl;

            Json::Value rules;
            m_protocol_json["protocol"] = prot["protocol"];
            m_protocol_json["access"] = prot["access"];


            if (prot["prefix"].empty() and prot["rules"].empty())
            {
                eDest->Say("No rule nor prefix specified for ", m_protocol.c_str());
                return XRDCMSJSON_ERR_URL;
            }

            // PREFIX
            if (!prot["prefix"].empty())
            {
                std::string new_pfn = prot["prefix"].asString() + "/$1";
                appendRuleJson("(/.*)", new_pfn, &rules);
                m_protocol_json["rules"] = rules;
                break;

            } 
            // RULES
            else if (!prot["rules"].empty())
            {
                for(const auto& rule_defined : prot["rules"])
                {
                    // CHAINED
                    if (!rule_defined["chain"].empty())
                    {
                        //std::cout << "Chain: " << rule_defined["chain"] << std::endl;
                        for(const auto& protocol_chain : actualJson[0]["protocols"])
                        { 
                            if (rule_defined["chain"].asString() == protocol_chain["protocol"].asString())
                            {
                                for(const auto& rule_chain : protocol_chain["rules"])
                                {
                                    std::string new_pfn = resolveChain(rule_chain, rule_defined);
                                    appendRuleJson(rule_chain["lfn"].asString(), new_pfn, &rules);
                                }
                                break;
                            }
                        }
                    } 
                    // NON-CHAINED
                    else
                    {
                        appendRuleJson(rule_defined["lfn"].asString(), rule_defined["pfn"].asString(), &rules);
                    }
                }
            }
            
            m_protocol_json["rules"] = rules;
            break;
        }
    }

    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::lfn2pfn(const char *lfn, char *buff, int blen)
{
    std::string tmp_lfn = lfn;
    std::string tmp_pfn = "";
    eDest->Say("LFN to translate: ", tmp_lfn.c_str());

    // First checks
    std::regex namesp_cms ("/store/.*");
    if (!regex_match (tmp_lfn.c_str(), namesp_cms))
    {
        eDest->Say("You need to use the CMS namespace '/store/...'");
        return XRDCMSJSON_ERR_FILE;
    }

    bool rule_found = false;
    std::cout << "Rules: " <<  m_protocol_json["rules"] << std::endl;

    for(const auto& rule : m_protocol_json["rules"])
    {
        std::regex lfn_expr (rule["lfn"].asString());
        std::cout << "Trying... : " << rule["lfn"].asString() << std::endl;
        if (regex_match (tmp_lfn.c_str(),lfn_expr))
        {
            // If the rule match with our LFN -> create the PFN
            std::string pfn = rule["pfn"].asString();
            // Replace /$1 -> lfn
            pfn = regex_replace(tmp_lfn.c_str(), lfn_expr, pfn);
            rule_found = true;
            std::cout <<"PFN translation: " << pfn << std::endl;
            break;
        } else
        {
            std::cout << "REGEX DO NOT MATCH " << rule["lfn"].asString() << std::endl;
        };
    };

    if (!rule_found) 
    {
        eDest->Say("None rule match with the LFN: ", tmp_lfn.c_str());
        return XRDCMSJSON_ERR_NOLFN2PFN;
    };

    return 0;
}

int XrdCmsJson::PathTranslation::pfn2lfn(const char *pfn, char *buff, int blen) 
{
   return 0;
}


int XrdCmsJson::PathTranslation::lfn2rfn(const char *pfn, char *buff, int blen) 
{
    return 0;
}

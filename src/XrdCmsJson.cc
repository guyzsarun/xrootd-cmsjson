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

int XrdCmsJson::PathTranslation::parse ()
{
    eDest->Say("Conecting to the catalog ", m_url.c_str());
    m_filename = m_url.substr(0, m_url.find("?"));
    m_protocol = m_url.substr(m_url.find("protocol=")+9 ,m_url.length()); 
    m_volume = m_url.substr(m_url.find("volume=")+7 ,m_url.find("&protocol=")-(m_url.find("volume=")+7) ); 
    

    std::cout << "Volume " << m_volume << ", protocol " << m_protocol << std::endl;

    // First checks
    if (m_filename.empty() or m_protocol.empty()) {
        eDest->Say("PathTranslation::connect: Malformed url for file catalog configuration");
        return XRDCMSJSON_ERR_URL;   
    }

    // Load json
    std::ifstream file("/root/xrootd-cmsjson/storage_test.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse( file, actualJson);

    if (actualJson.empty()){
        eDest->Say("The storage.json is empty or the path is incorrect. Information has not ben loaded");
        return XRDCMSJSON_ERR_URL; 
    }

    // Find desired protocol
    for(const auto& prot : actualJson[0]["protocols"]){
        if (prot["protocol"] == m_protocol) {
            // Info type access 
            std::cout << "Protocol " << m_protocol << " with access: " << prot["access"].asString() << std::endl;

            Json::Value rules;
            m_protocol_json["protocol"] = prot["protocol"];
            m_protocol_json["access"] = prot["access"];


            if (prot["prefix"].empty() and prot["rules"].empty()){
                eDest->Say("No rule nor prefix specified for ", m_protocol.c_str());
                return XRDCMSJSON_ERR_URL;
            }
            if (!prot["prefix"].empty()){
                rules.append("lfn") = "(.*)";
                rules.append("pfn") = prot["prefix"]; //add $1
                std::cout << "PFN 2 json: " << rules.asString() << std::endl;
                m_protocol_json["rules"] = rules;
            }
            
            //for(const auto& rule : prot["rules"]){
            if (!prot["rules"][0]["chain"].empty()){
                std::string chain_name = prot["rules"][0]["chain"].asString();
                std::cout << "Rule: " << chain_name << std::endl;
                for(const auto& prot_chain : actualJson[0]["protocols"]){
                    if (prot_chain["protocol"] == chain_name) {
                        for(const auto& rule_chain : prot_chain["rules"]){
                            
                            std::string str_rule_main = prot["rules"][0]["pfn"].asString();
                            std::string str_rule_chain =rule_chain["pfn"].asString();

                            std::string pfn = prot["rules"][0]["pfn"].asString();
                            std::regex lfn_expr (prot["rules"][0]["lfn"].asString());

                            std::string str_lfn_chain =rule_chain["lfn"].asString();

                            pfn = regex_replace(str_rule_chain, lfn_expr, pfn);
                            std::cout << "main_lfn: " << prot["rules"][0]["lfn"].asString() << std::endl;
                            std::cout << "rule_main: " << str_rule_main << std::endl;
                            std::cout << "rule_chain: " << str_rule_chain << std::endl;
                            std::cout << "chain_lfn: " << str_lfn_chain << std::endl;
                            std::cout << "Result chain: " << pfn << std::endl;
                            //Json::Value chained;
                            //chained.append("pfn") = str_rule_chain.append(str_rule_main);
                            //rule_chain["pfn"] = chained.;
                        }
                    m_protocol_json["rules"] = prot_chain["rules"];
                    break;
                    }

                }                
            }else {
                m_protocol_json["rules"] = prot["rules"];
            }

            
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
    if (!regex_match (tmp_lfn.c_str(), namesp_cms)){
        eDest->Say("You need to use the CMS namespace '/store/...'");
        return XRDCMSJSON_ERR_FILE;
    }
    

    bool rule_found = false;
    std::cout << "Rules: " <<  m_protocol_json["rules"] << std::endl;

    for(const auto& rule : m_protocol_json["rules"]){
        std::regex lfn_expr (rule["lfn"].asString());
        std::cout << "Trying... : " << rule["lfn"].asString() << std::endl;
        if (regex_match (tmp_lfn.c_str(),lfn_expr)){
            // If the rule match with our LFN -> create the PFN
            std::string pfn = rule["pfn"].asString();
            // Replace /$1 -> lfn
            pfn = regex_replace(tmp_lfn.c_str(), lfn_expr, pfn);
            rule_found = true;
            std::cout <<"PFN translation: " << pfn << std::endl;
            break;
        }else{
            std::cout << "REGEX DO NOT MATCH " << rule["lfn"].asString() << std::endl;
        };
    };
    if (!rule_found) {
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


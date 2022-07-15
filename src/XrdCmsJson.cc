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

XrdCmsJson::PathTranslation::PathTranslation(XrdSysError *lp, const char * tfc_file)
{
    m_url = tfc_file;
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
            m_protocol_json["rules"] = prot["rules"];


            if (prot["prefix"].empty() and prot["rules"].empty()){
                eDest->Say("No rule nor prefix specified for ", m_protocol.c_str());
                return XRDCMSJSON_ERR_URL;
            }
            if (!prot["prefix"].empty()){
                rules.append("lfn") = "(.*)";
                rules.append("pfn") = prot["prefix"];
                std::cout << "PFN 2 json: " << rules.asString() << std::endl;
                m_protocol_json["rules"] = rules;
            }
            break;
        }
    }

    return 0;
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


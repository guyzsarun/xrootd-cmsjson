#include "XrdCmsTfc.hh"
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

using namespace XrdCmsTfc;

XrdCmsTfc::TrivialFileCatalog::TrivialFileCatalog(XrdSysError *lp, const char * tfc_file)
{
    m_url = tfc_file;
    eDest = lp;
    parse();
}

int XrdCmsTfc::TrivialFileCatalog::parse ()
{
    eDest->Say("Conecting to the catalog ", m_url.c_str());

    if (m_url.find ("file:") != std::string::npos) {
        // remove file:// -> position : +3 i.e. /root/storage.json?protocol=srm
        m_url = m_url.erase (0, m_url.find(":") +3 );
        m_filename = m_url.substr(0, m_url.find("?"));
        m_protocol = m_url.substr(m_url.find("=")+1 ,m_url.length());
    } else {
            eDest->Say("TrivialFileCatalog::connect: Malformed url for file catalog configuration");
            return XRDCMSTFC_ERR_URL;
	}
    return 0;
}

int XrdCmsTfc::TrivialFileCatalog::lfn2pfn(const char *lfn, char *buff, int blen)
{
    // Initial conditions
    if (m_filename.empty()){
        eDest->Say("The path to the storage.json has not been provided");
        return XRDCMSTFC_ERR_URL;
    }
    if (m_protocol.empty()){
        eDest->Say("No protocol has been provided");
        return XRDCMSTFC_ERR_URL;
    }

    std::string tmp_lfn = lfn;
    std::string tmp_pfn = "";
    eDest->Say("LFN to translate: ", tmp_lfn.c_str());
    
    std::ifstream file(m_filename);
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse( file, actualJson);

    bool protocol_found = false;
    for(const auto& prot : actualJson[0]["protocols"]){
        if (prot["protocol"] == m_protocol) {
            // We will use the protocol specified on the site-local-config
            if (prot["prefix"].empty()){
                // If it doesn't use a prefix means that we will need rules
                if (prot["rules"].empty()){
                    // If there are no prefix no rules -> ERROR
                    return XRDCMSTFC_ERR_PARSERULE;
                } else {
                    // If there are rules -> check what rule match with the LFN
                    for(const auto& rule : prot["rules"]){
                        std::regex rgx_expr (rule["lfn"].asString());
                        if (regex_match (tmp_lfn,rgx_expr)){
                            // If the rule match with our LFN -> create the PFN
                            std::string prefix = rule["pfn"].asString();
                            // Replace /$1 -> lfn
                            std::regex reg("\\/\\$1");
                            prefix = std::regex_replace(prefix, std::regex(reg), tmp_lfn.c_str());
                            protocol_found = true;
                            std::cout << "PFN translation:" << prefix << std::endl;
                        }else{
                            std::cout << "REGEX DO NOT MATCH " << rule["lfn"].asString() << std::endl;
                        };
                    };
                };
            } else {
                std::string prefix = prot["prefix"].asString();
                if (prefix.empty()){
                    eDest->Say("No rule nor prefix specified for ", m_protocol.c_str());
                    return XRDCMSTFC_ERR_URL;
                } else {
                    protocol_found = true;
                    prefix = prefix.append(tmp_lfn.c_str());
                    std::cout << "PFN translation:" << prefix << std::endl;
                }
            };
        };
    };

    if (!protocol_found) {
        eDest->Say("The protocol has not been found: ", m_protocol.c_str());
        return XRDCMSTFC_ERR_NOLFN2PFN;
    };



    return 0;
}


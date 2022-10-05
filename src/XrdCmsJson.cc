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
    // Test all "required" paths so site admins can see if there is any wrong configuration 
    for(const auto lfn_area : CMS_ALL_NAMESPACES)
    {
        int blen = 4096;
        char* buff = (char*) malloc(blen);
        const char* n_lfn =  lfn_area.c_str();
        this->lfn2pfn(n_lfn, buff, blen);
    }
}

int XrdCmsJson::PathTranslation::appendRuleJson (std::string lfn, std::string pfn, Json::Value *m_rules)
{
    Json::Value rule;
    rule["lfn"] = lfn;
    rule["pfn"] = pfn;
    m_rules->append(rule);
    return 0;
}

Json::Value XrdCmsJson::PathTranslation::buildRule (std::string lfn, std::string pfn, std::string chain)
{
    Json::Value rule;
    rule["lfn"] = lfn;
    rule["pfn"] = pfn;
    if (!chain.empty())
    {
        rule["chain"] = chain;
    }
    return rule;
}

std::string XrdCmsJson::PathTranslation::resolveChain(Json::Value rule_root, Json::Value rule_chain)
{
    //"davs://my.domain.ch:2880/$1"
    std::string pfn = rule_root["pfn"].asString();
    //"/+(.*)"
    std::regex lfn_expr (rule_root["lfn"].asString());
    // "/pnfs/$1"
    std::string pfn_chain = rule_chain["pfn"].asString();
    // "davs://my.domain.ch:2880/pnfs/$1"
    std::string pfn_resolved = regex_replace(pfn_chain, lfn_expr, pfn);
    return pfn_resolved;
}

Json::Value XrdCmsJson::PathTranslation::parseChain(Json::Value rule_root, Json::Value rule_chain) 
{
    // CHAINED
    std::string pfn_resolved = resolveChain(rule_root, rule_chain);
    Json::Value r_rule_chained = buildRule(rule_chain["lfn"].asString(), pfn_resolved, rule_chain["chain"].asString());
    return r_rule_chained;

}

int XrdCmsJson::PathTranslation::verifyFormatRule(Json::Value rule) 
{
    if (rule["lfn"].empty() or rule["pfn"].empty())
    {
        eDest->Say("Rule doesn't have PFN or LFN ");
        return XRDCMSJSON_ERR_RULE;
    }
    return XRDCMSJSON_OK;   
}

Json::Value XrdCmsJson::PathTranslation::simplifyProtocol(Json::Value protocol) 
{
    // If the upper rule includes the lower ones, they will never be used --> trigger alarm
    if (!protocol["rules"].empty())
    {
        int num_rules = protocol["rules"].size();
        for( int idx_rule = 0; idx_rule < num_rules; idx_rule++)
        {
            for (int idx_subrule = idx_rule+1; idx_subrule < num_rules; idx_subrule++) 
            {
                std::string lfn_nextrule = protocol["rules"][idx_subrule]["lfn"].asString();
                std::regex rgx_lfn_nextrule(lfn_nextrule);
                
                std::cout << " pro " << protocol["rules"][idx_rule]["lfn"].asString() << "RGX " << lfn_nextrule.c_str() << std::endl;
                
                if (regex_match(protocol["rules"][idx_rule]["lfn"].asString(), rgx_lfn_nextrule));
                {
                    std::cout << "UNUSED RULE INDEX " << idx_subrule << std::endl;
                }
            }
        }
    
    }
    return protocol;
}


int XrdCmsJson::PathTranslation::verifyFormatProtocol(Json::Value prot) 
{
    //std::cout << "Protocol " << m_protocol << " with access: " << m_json[m_protocol]["access"].asString() << std::endl;

    if (prot["prefix"].empty() and m_json[m_protocol]["rules"].empty())
    {
        eDest->Say("No rule nor prefix specified for ");
        return XRDCMSJSON_ERR_PROTOCOL;
    } 

    return XRDCMSJSON_OK;
}

Json::Value XrdCmsJson::PathTranslation::parsePrefix(std::string pfn)
{
    std::string prefix_pfn = pfn + "$1";
    std::string empty_str;
    Json::Value rule_built = buildRule("(/.*)", prefix_pfn , empty_str);
    return rule_built;
}

std::string XrdCmsJson::PathTranslation::matchLFN(Json::Value rule, std::string target_lfn)
{
        std::string pfn;
        std::regex lfn_expr (rule["lfn"].asString());
        if (regex_match (target_lfn.c_str(),lfn_expr))
        {
            // If the rule match with our LFN -> create the PFN
            pfn = rule["pfn"].asString();
            // Replace /$1 -> lfn
            pfn = regex_replace(target_lfn.c_str(), lfn_expr, pfn);
        }
        return pfn;
}


Json::Value XrdCmsJson::PathTranslation::parseProtocol(Json::Value protocol, std::string lfn) 
{
    std::string name_chain = protocol["protocol"].asString();

    for( const auto& rule_chain : protocol["rules"])
    {
        Json::Value new_rule_chain;
        std::string pfn_init = matchLFN(rule_chain, lfn);
        // the rule is chained and the lfn can be resolved (upstream) -> go deeper
        if (!rule_chain["chain"].empty() && !pfn_init.empty())
        {
            std::string name_chain2 = rule_chain["chain"].asString();
            new_rule_chain = parseProtocol(m_json[name_chain2], lfn);
        }
        if (!new_rule_chain["pfn_chain"].empty())
        {
            lfn = new_rule_chain["pfn_chain"].asString();
        }
        std::string pfn_result = matchLFN(rule_chain, lfn);
        if (!pfn_result.empty())
        {
            Json::Value rule_with_chain = rule_chain;
            rule_with_chain["pfn_chain"] = pfn_result;
            return rule_with_chain;
        }
    }
    Json::Value exit_value;
    return exit_value;

}


int XrdCmsJson::PathTranslation::parseUrl() 
{
    eDest->Say("Conecting to the catalog ", m_url.c_str());
    // TODO: CHANGE 5 -> REMOVE file:
    m_filename = m_url.substr(5, m_url.find("?")-5);
    m_protocol = m_url.substr(m_url.find("protocol=")+9 ,m_url.length()); 
    m_volume = m_url.substr(m_url.find("volume=")+7 ,m_url.find("&protocol=")-(m_url.find("volume=")+7) );
    
    std::cout << "Volume " << m_volume << ", protocol " << m_protocol << std::endl;

    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::verifyFormatURL() 
{
    // TODO: check site (if none --> the one defined on the site)

    if (m_filename.empty() or m_protocol.empty()) 
    {
        eDest->Say("PathTranslation::connect: Malformed url for file catalog configuration");
        return XRDCMSJSON_ERR_URL;   
    }
}

// Reformat json to use protocol name as key
int XrdCmsJson::PathTranslation::reformatJson(Json::Value original_storage)
{
    for(const auto& prot : original_storage[0]["protocols"])
    {
        std::string protocol_name = prot["protocol"].asString();
        m_json[protocol_name] = prot;
    }
    return XRDCMSJSON_OK;
}


int XrdCmsJson::PathTranslation::parseStorageJson()
{
    // Load json
    std::ifstream file(m_filename);
    Json::Value storageJson;
    Json::Reader reader;
    reader.parse( file, storageJson);
    reformatJson(storageJson);
    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::verifyFormatJson() 
{
    if (m_json.empty())
    {
        eDest->Say("The storage.json is empty or the path is incorrect. Information has not ben loaded");
        return XRDCMSJSON_ERR_JSON; 
    }

    return XRDCMSJSON_OK;
}

// Parse and verify protocol data and store clean information inside m_rules
int XrdCmsJson::PathTranslation::parse()
{

    parseUrl();
    verifyFormatURL();

    parseStorageJson();
    verifyFormatJson();
    verifyFormatProtocol(m_json[m_protocol]);
    //Json::Value prot = simplifyProtocol(m_json[m_protocol]);
    //Json::Value r_all_rules = parseProtocol(m_json[m_protocol]);    
    // PREFIX
    if (!m_json[m_protocol]["prefix"].empty())
    {
        Json::Value rule_pfx = parsePrefix(m_json[m_protocol]["prefix"].asString());
        // Prefix put in "rule format"
        m_rules.append(rule_pfx);
    } else 
    {
        m_rules = m_json[m_protocol]["rules"];
    }
    
    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::lfn2pfn(const char *lfn, char *buff, int blen)
{
    std::string target_lfn = lfn;

    eDest->Say("LFN to translate: ", target_lfn.c_str());

    // First checks
    std::regex rgx_cms_namesp(CMS_NAMESPACE);
    if (!regex_match (target_lfn.c_str(), rgx_cms_namesp))
    {
        eDest->Say("You need to use the CMS namespace '/store/...'");
        return XRDCMSJSON_ERR_FORMAT;
    }
    bool rule_found = false;
    for(const auto& rule : m_rules)
    {
        std::regex lfn_expr (rule["lfn"].asString());
        if (!rule["chain"].empty())
        {
            std::string chain_name = rule["chain"].asString();
            Json::Value rule_chain = parseProtocol(m_json[chain_name], target_lfn.c_str());
            if (!rule_chain["pfn_chain"].empty())
            {
                Json::Value lfn_result_chain = rule_chain["pfn_chain"].asString();
                target_lfn = lfn_result_chain.asString();
            }
        }
        std::string pfn_result = matchLFN(rule, target_lfn);

        if (!pfn_result.empty())
        {
            std::cout << "Final Result: " <<  pfn_result << std::endl;
            return XRDCMSJSON_OK;
        }
    };

    if (!rule_found) 
    {
        eDest->Say("None rule match with the LFN: ", target_lfn.c_str());
        return XRDCMSJSON_ERR_DATAMISSING;
    };

}

int XrdCmsJson::PathTranslation::pfn2lfn(const char *pfn, char *buff, int blen) 
{
   return XRDCMSJSON_OK;
}


int XrdCmsJson::PathTranslation::lfn2rfn(const char *pfn, char *buff, int blen) 
{
    return XRDCMSJSON_OK;
}
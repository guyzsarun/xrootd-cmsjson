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
    //testCMSNamespaces();

}

int XrdCmsJson::PathTranslation::testCMSNamespaces ()
{
    // Test all "required" paths so site admins can see if there is any wrong configuration
    for(const auto lfn_area : CMS_ALL_NAMESPACES)
    {
        int blen = 4096;
        char* buff = (char*) malloc(blen);
        const char* n_lfn =  lfn_area.c_str();
        this->lfn2pfn(n_lfn, buff, blen);
    }
    return 0;
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

                //std::cout << " pro " << protocol["rules"][idx_rule]["lfn"].asString() << "RGX " << lfn_nextrule.c_str() << std::endl;

                //if (regex_match(protocol["rules"][idx_rule]["lfn"].asString(), rgx_lfn_nextrule));
                //{
                //    std::cout << "UNUSED RULE INDEX " << idx_subrule << std::endl;
                //}
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
        eDest->Say("No rule nor prefix specified");
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
        std::regex lfn_expr(rule["lfn"].asString() == "(.*)" ? "(.+)" :rule["lfn"].asString());
        if (regex_match (target_lfn.c_str(),lfn_expr))
        {
            pfn = rule["pfn"].asString();
            pfn = regex_replace(target_lfn.c_str(), lfn_expr, pfn);
        }
        return pfn;
}


Json::Value XrdCmsJson::PathTranslation::parseProtocol(Json::Value rules, std::string lfn)
{
    for( const auto& rule : rules)
    {
        Json::Value new_rule;
        std::string pfn_init = matchLFN(rule, lfn);
        // the rule is chained and the lfn can be resolved (upstream) -> go deeper
        if (!rule["chain"].empty() && !pfn_init.empty())
        {
            std::string name_chain = rule["chain"].asString();
            new_rule = parseProtocol(m_json[name_chain]["rules"], lfn);
        }
        if (!new_rule["pfn_chain"].empty())
        {
            lfn = new_rule["pfn_chain"].asString();
        }

        std::string pfn_result = matchLFN(rule, lfn);

        //std::cout << rule << "\n" << new_rule << " " << lfn << "\n" << new_rule["pfn_chain"] << "<->" << pfn_result << std::endl;

        if (!new_rule["pfn_chain"].empty() and pfn_result.empty()){
            Json::Value rule_with_chain = rule;
            rule_with_chain["pfn_chain"] = rule["pfn"];
            return rule_with_chain;
        }

        if (!pfn_result.empty())
        {
            Json::Value rule_with_chain = rule;
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
    m_filename = m_url.substr(5, m_url.find("?")-5);
    m_protocol = m_url.substr(m_url.find("protocol=")+9 ,m_url.length());
    m_volume = m_url.substr(m_url.find("volume=")+7 ,m_url.find("&protocol=")-(m_url.find("volume=")+7) );

    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::verifyFormatURL()
{

    if (m_filename.empty() or m_protocol.empty())
    {
        eDest->Say("PathTranslation::connect: Malformed url for file catalog configuration");
        return XRDCMSJSON_ERR_URL;
    }
    return XRDCMSJSON_OK;
}

// Reformat json to use protocol name as key
int XrdCmsJson::PathTranslation::reformatJson(Json::Value original_storage)
{
    for (const auto& volume_json : original_storage)
    {
        if (volume_json["volume"] == m_volume){

            for(const auto& prot : volume_json["protocols"])
            {
                std::string protocol_name = prot["protocol"].asString();
                m_json[protocol_name] = prot;
            }
            return XRDCMSJSON_OK;
        }
    }
    eDest->Say("None Volume has matched with the existing ones");
    return XRDCMSJSON_ERR_JSON;
}


int XrdCmsJson::PathTranslation::parseStorageJson()
{
    // Load json
    std::ifstream file(m_filename);
    Json::Value storageJson;
    Json::CharReaderBuilder jsonReader;
    std::string errs;
    Json::parseFromStream(jsonReader, file, &storageJson, &errs);
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

    // If it is a prefix, we transform it to a rule format
    if (!m_json[m_protocol]["prefix"].empty())
    {
        Json::Value rule_pfx = parsePrefix(m_json[m_protocol]["prefix"].asString());
        m_rules.append(rule_pfx);
    } else
    {
        m_rules = m_json[m_protocol]["rules"];
    }

    //std::cout << "active rules " << m_rules << std::endl;
    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::lfn2pfn(const char *lfn, char *buff, int blen)
{
    std::string target_lfn = lfn;
    //eDest->Say("LFN to translate: ", target_lfn.c_str());
    //std::cout << "m_rules" << m_rules <<std::endl;
    Json::Value rule_result = parseProtocol(m_rules, target_lfn.c_str());
    // If it is using a chain, the LFN will be the last LFN downstream
    std::string pfn_result;

    if (!rule_result["pfn_chain"].empty())
    {
        pfn_result = rule_result["pfn_chain"].asString();
    }else {
        pfn_result = matchLFN(rule_result, target_lfn);
    }

    if (!pfn_result.empty())
    {
        strncpy(buff,pfn_result.c_str(),blen);
        return XRDCMSJSON_OK;
    }

    eDest->Say("No lfn2pfn mapping for: ", lfn);
    strncpy(buff,lfn,blen);
    return XRDCMSJSON_OK;
}

int XrdCmsJson::PathTranslation::pfn2lfn(const char *pfn, char *buff, int blen)
{
   return XRDCMSJSON_OK;
}


int XrdCmsJson::PathTranslation::lfn2rfn(const char *lfn, char *buff, int blen)
{
    return lfn2pfn(lfn, buff, blen);
}
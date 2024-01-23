#include "api.hh"
#include <vector>
#include <iostream>
#include <stdexcept>

#include <cpr/cpr.h>
#include <json/json.h>

using std::cerr, std::endl;
using std::string, std::vector;
using error = std::runtime_error;

namespace MWAPI
{
  string api_base_url("https://zh.wikipedia.org/w/api.php");

  static string get_page_wikitext(const string page_name);

  string page_wikitext(string page_name)
  {
    const string resp = get_page_wikitext(page_name);
    Json::Value root;

    if (!Json::Reader().parse(resp, root))
    {
      throw error("invalid json");
    }

    if (root["error"])
    {
      throw error("MediaWiki error: " + root["error"]["info"].asString());
    }

    return root["parse"]["wikitext"].asString();
  }

  static string get_page_wikitext(const string page_name)
  {
    auto resp = cpr::Get(cpr::Url(api_base_url),
                         cpr::Parameters{
                             {"action", "parse"},
                             {"redirects", "true"},
                             {"page", page_name},
                             {"prop", "wikitext"},
                             {"maxlag", "5"},
                             {"format", "json"},
                             {"formatversion", "2"},
                         },
                         cpr::UserAgent("wikitext-bot-fix/1.0"));

    return resp.text;
  }
} // namespace MWAPI

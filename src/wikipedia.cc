#include "wikipedia.hh"
#include <vector>
#include <iostream>
#include <stdexcept>

#include <cpr/cpr.h>
#include <jsoncpp/json/json.h>

using std::cerr, std::endl;
using std::string, std::vector;
using error = std::runtime_error;

namespace Wikipedia
{
  static string get_page_wikitext(const string page_name);

  string page_wikitext(string page_name)
  {
    const string resp = get_page_wikitext(page_name);
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(resp, root))
    {
      throw error("invalid json");
    }

    return root["parse"]["wikitext"].asString();
  }

  static string get_page_wikitext(const string page_name)
  {
    auto resp = cpr::Get(cpr::Url(WIKIPEDIA_API_URL),
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
} // namespace Wikipedia

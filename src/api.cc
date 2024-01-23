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
  string api_user_agent("wikitext-bot-fix/1.0");
  string api_csrf_token("");

  static Json::Value parse(const string text)
  {
    static Json::Reader json;
    Json::Value root;
    if (!json.parse(text, root))
    {
      throw error("invalid json");
    }
    if (root["error"])
    {
      throw error("MediaWiki error: " + root["error"]["info"].asString());
    }
    return root;
  }

  using Dict = vector<std::pair<string, string>>;

  Dict common_params{
      {"maxlag", "5"},
      {"format", "json"},
      {"formatversion", "2"}};

  template <typename T>
  T &apply_common_params(T &pairs)
  {
    for (const auto &param : common_params)
      pairs.Add({param.first, param.second});
    return pairs;
  }

  void populate_csrf_token()
  {
    cpr::Parameters params{
        {"action", "query"},
        {"meta", "tokens"}};

    auto resp = cpr::Get(
        cpr::Url(api_base_url),
        apply_common_params(params),
        cpr::UserAgent(api_user_agent));

    api_csrf_token = parse(resp.text)["query"]["tokens"]["csrftoken"].asString();
  }

  string get_page_content(string title)
  {
    cpr::Parameters params{
        {"action", "parse"},
        {"page", title},
        {"prop", "wikitext"},
        {"redirects", "true"}};

    auto resp = cpr::Get(
        cpr::Url(api_base_url),
        apply_common_params(params),
        cpr::UserAgent(api_user_agent));

    return parse(resp.text)["parse"]["wikitext"].asString();
  }
  string edit_page(string title, string content, string summary, bool minor)
  {
    cpr::Payload payload{
        {"action", "edit"},
        {"title", title},
        {"text", content},
        {"summary", summary},
        {"bot", "true"},
        {"minor", minor ? "true" : ""},
        {"token", api_csrf_token}};

    auto resp = cpr::Post(
        cpr::Url(api_base_url),
        apply_common_params(payload),
        cpr::UserAgent(api_user_agent));

    return parse(resp.text)["edit"]["newrevid"].asString();
  }
} // namespace MWAPI

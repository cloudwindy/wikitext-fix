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
  using Dict = vector<std::pair<string, string>>;
  const static Dict common_params{
      {"maxlag", "5"},
      {"format", "json"},
      {"formatversion", "2"},
      {"uselang", "zh"},
      {"variant", "zh-cn"},
      {"errorformat", "plaintext"},
      {"errorsuselocal", "true"}};

  static Json::Value json_parse(const string text);
  template <class T>
  static T append_common_params(T pairs);

  API::API(string base_url, string user_agent)
  {
    sess.SetUrl(base_url);
    sess.SetUserAgent(user_agent);
    // sess.SetVerbose(cpr::Verbose(true));
  }

  template <typename T>
  void API::sess_set_params(T params)
  {
    for (const auto &param : common_params)
      params.Add({param.first, param.second});
    sess.SetOption(params);
  }
  template <typename T>
  void API::sess_clear_params()
  {
    T t{};
    sess.SetOption(t);
  }

  string API::get_page_content(string title)
  {
    sess_set_params(cpr::Payload{
        {"action", "parse"},
        {"page", title},
        {"prop", "wikitext"},
        {"redirects", "true"}});
    auto resp = sess.Post();
    sess_clear_params<cpr::Payload>();

    return json_parse(resp.text)["parse"]["wikitext"].asString();
  }

  string API::login(string username, string password)
  {
    populate_login_token();

    sess_set_params(cpr::Payload{
        {"action", "login"},
        {"lgname", username},
        {"lgpassword", password},
        {"lgtoken", login_token}});
    auto resp = sess.Post();
    sess_clear_params<cpr::Payload>();

    auto res = json_parse(resp.text);
    if (res["login"]["result"] != "Success")
    {
      std::ostringstream err;
      err << "MediaWiki login message: \"" << res["login"]["reason"]["text"].asString() << '"';
      throw error(err.str());
    }

    return res["login"]["lgusername"].asString();
  }

  string API::edit_page(string title, string content, string summary, bool minor)
  {
    populate_csrf_token();

    sess_set_params(cpr::Payload{
        {"action", "edit"},
        {"title", title},
        {"text", content},
        {"summary", summary},
        {"bot", "true"},
        {"minor", minor ? "true" : ""},
        {"token", csrf_token}});
    auto resp = sess.Post();
    sess_clear_params<cpr::Payload>();

    auto res = json_parse(resp.text);
    if (res["edit"]["nochange"])
    {
      throw error("page not changed");
    }

    return res["edit"]["newrevid"].asString();
  }

  void API::populate_csrf_token()
  {
    if (!csrf_token.empty())
      return;

    sess_set_params(cpr::Parameters{
        {"action", "query"},
        {"meta", "tokens"}});
    auto resp = sess.Get();
    sess_clear_params<cpr::Parameters>();

    csrf_token = json_parse(resp.text)["query"]["tokens"]["csrftoken"].asString();
  }

  void API::populate_login_token()
  {
    if (!login_token.empty())
      return;

    sess_set_params(cpr::Parameters{
        {"action", "query"},
        {"meta", "tokens"},
        {"type", "login"}});
    auto resp = sess.Get();
    sess_clear_params<cpr::Parameters>();

    login_token = json_parse(resp.text)["query"]["tokens"]["logintoken"].asString();
  }

  static Json::Value json_parse(const string text)
  {
    static Json::Reader json;
    Json::Value root;
    if (!json.parse(text, root))
    {
      std::ostringstream err;
      err << "invalid json size=" << text.size() << " text=\"" << text << '"';
      throw error(err.str());
    }
    if (root["errors"])
    {
      std::ostringstream err;
      err << "MediaWiki returned \"" << root["errors"][0]["text"].asString() << '"';
      throw error(err.str());
    }
    return root;
  }
} // namespace MWAPI

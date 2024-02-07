#pragma once

#include <string>
#include <cpr/cpr.h>

namespace MWAPI
{
  const std::string default_base_url = "https://zh.wikipedia.org/w/api.php";
  const std::string default_ua = "wikitext-bot-fix/1.0";
  class API
  {
  public:
    API(std::string base_url = default_base_url, std::string user_agent = default_ua);
    std::string get_page_content(std::string page_name);
    std::string login(std::string username, std::string password);
    std::string edit_page(std::string title, std::string content, std::string summary = "", bool minor = false);

  private:
    cpr::Session sess;
    std::string csrf_token;
    std::string login_token;
    template <typename T>
    void sess_set_params(T);
    template <typename T>
    void sess_clear_params();
    void populate_csrf_token();
    void populate_login_token();
  };
} // namespace MWAPI

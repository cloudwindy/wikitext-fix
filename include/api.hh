#pragma once

#include <string>

namespace MWAPI
{
  extern std::string api_base_url;
  void populate_csrf_token();
  std::string get_page_content(std::string page_name);
  std::string edit_page(std::string title, std::string content, std::string summary = "", bool minor = false);
} // namespace MWAPI

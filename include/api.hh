#pragma once

#include <string>

#define WIKIPEDIA_API_URL "https://zh.wikipedia.org/w/api.php"

namespace MWAPI
{
  std::string page_wikitext(std::string page_name);
} // namespace MWAPI

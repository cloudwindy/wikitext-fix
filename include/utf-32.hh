#pragma once

#include <locale>

struct destructible_codecvt : public std::codecvt<char32_t, char, std::mbstate_t>
{
  using std::codecvt<char32_t, char, std::mbstate_t>::codecvt;
  ~destructible_codecvt() = default;
};
extern std::wstring_convert<destructible_codecvt, char32_t> converter;

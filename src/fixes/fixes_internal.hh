#pragma once

#include <iostream>
#include <ranges>

#include "utf-32.hh"
#include "fix.hh"

namespace Fixes
{
  using namespace std::views;
  using namespace std::string_view_literals;

  using std::cout, std::cerr, std::endl;
  using std::string;
  using ustring = std::u32string;

  bool is_digit(char32_t ch);
  bool is_alnum(char32_t ch);
  bool is_symbol(char32_t ch);
  bool is_space(char32_t ch);
  bool is_ln(char32_t ch);
  bool is_blk_text(const Wiki::Block &blk);
  bool is_blk_visible(const Wiki::Block &blk);
  bool is_blk_ref(const Wiki::Block &blk);
};
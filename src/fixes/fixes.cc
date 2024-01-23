#include "fixes_internal.hh"

namespace Fixes
{
  bool is_blk_text(const Wiki::Block &blk)
  {
    return blk.type == WikiParser::TEXT;
  };
  bool is_blk_visible(const Wiki::Block &blk)
  {
    return blk.type == WikiParser::TEXT || blk.type == WikiParser::LINK;
  };
  bool is_blk_ref(const Wiki::Block &blk)
  {
    return (blk.type == WikiParser::HTML_TAG || blk.type == WikiParser::HTML_SELF_CLOSING_TAG) && blk.value.starts_with(U"ref");
  };
  bool is_space(char32_t ch)
  {
    return ch == U' ';
  };
  bool is_ln(char32_t ch)
  {
    return ch == U'\n';
  };
  bool is_digit(char32_t ch)
  {
    return ((ch - '0' >= 0) && (ch - '0' < 10));
  }
  bool is_alnum(char32_t ch)
  {
    return (ch - 'a' >= 0) && (ch - 'a' < 26) || (ch - 'A' >= 0) && (ch - 'A' < 26) || (ch - '0' >= 0) && (ch - '0' < 10);
  }
  bool is_symbol(char32_t ch)
  {
    return (ch >= '!') && (ch <= '/') || (ch >= ':') && (ch <= '@') || (ch >= '[') && (ch <= '`') || (ch >= '{') && (ch <= '}');
  }
}
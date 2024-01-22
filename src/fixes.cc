#include <set>
#include <iostream>
#include <algorithm>
#include <ranges>
#include <regex>

#include "utf-32.hh"
#include "fixes.hh"

using std::cout, std::cerr, std::endl;
using std::string;
using ustring = std::u32string;

namespace Fixes
{
  bool isdigit(char32_t ch);
  bool isalnum(char32_t ch);
  bool issymbol(char32_t ch);

  void footnotes(Wiki::Blocks &blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] :
         std::views::enumerate(blocks | std::views::slide(2)))
    {
      if (blk[0].type != WikiParser::TEXT)
        continue;

      const static ustring puncs = U"，。、！？；";
      const size_t punc_i = puncs.find(blk[0].value.back());
      if (punc_i == string::npos)
        continue;

      if ((blk[1].type != WikiParser::HTML_TAG &&
           blk[1].type != WikiParser::HTML_SELF_CLOSING_TAG) ||
          !blk[1].value.starts_with(U"ref"))
        continue;

      const auto tblk_next = std::ranges::find_if(
          std::views::drop(blocks, blk_i + 1),
          [](const auto &blk)
          { return blk.type == WikiParser::TEXT ||
                   blk.type == WikiParser::LINK; });
      // We're on last text block.
      if (tblk_next == blocks.end())
        break;

      const size_t text_next_i = std::distance(
          blocks.begin(),
          tblk_next);

      // Find stop marker.
      const std::set<ustring> stop_markers{
          U"\n",
          U"”",
          U"」",
          U"』",
      };
      const auto stop_next = std::ranges::find_if(
          stop_markers,
          [tblk = tblk_next](ustring stop)
          { return tblk->value.starts_with(stop); });
      if (stop_next != stop_markers.end())
        continue;

      // Remove period.
      blk[0].value.pop_back();
      fix_count++;

      const char32_t punc_ch = puncs.at(punc_i);

      if (puncs.find(tblk_next->value.front()) != string::npos)
      {
        cerr << "Remove '" << converter.to_bytes(punc_ch) << "' in blk " << blk_i << "." << endl;
        continue;
      }
      tblk_next->prepend = punc_ch;
      cerr << "Shift '" << converter.to_bytes(punc_ch)
           << "' from blk " << blk_i << " to blk " << text_next_i << "." << endl;
    }
  }

  void punctuation(Wiki::Blocks &blocks, int &fix_count)
  {
    int flag;
    for (const auto &&[blk_i, blk] : std::views::enumerate(blocks))
    {
      if (blk.type != WikiParser::TEXT ||
          blk.type != WikiParser::LINK)
        continue;

      const static ustring puncs = U"，。、“”【】！？『』〖〗：；「」・｜（）";

      ustring &str = blk.value;
      for (const auto &&[pos, c] : std::views::enumerate(str))
      {
        int flag_new = puncs.find(c);
        if (flag != string::npos && flag == flag_new)
        {
          str.erase(pos, 1);
          fix_count++;
          cerr << "Remove '" << converter.to_bytes(puncs[flag])
               << "' at " << pos << " of blk " << blk_i << "." << endl;
        }
        flag = flag_new;
      }
    }
  }

  void punctuation_width(Wiki::Blocks &blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] : std::views::enumerate(blocks))
    {
      if (blk.type != WikiParser::TEXT)
        continue;

      const static ustring puncs_en = U".,()＋．　丶";
      const static ustring puncs_zh = U"。，（）+. 、";

      ustring &str = blk.value;
      for (const auto &&[pos, c] : std::views::enumerate(str))
      {
        int flag = puncs_en.find(c);
        if (flag == string::npos)
          continue;

        bool front = pos != 0 && isalnum(str.at(pos - 1));
        bool back = pos != str.size() - 1 && isalnum(str.at(pos + 1));

        char32_t punc = puncs_zh.at(flag);

        if (punc != U'+' && punc != U'.' && punc != U' ' && front && back)
          continue;

        if (front && punc == U'。')
          continue;

        c = punc;
        fix_count++;
        cerr << "Replace '" << converter.to_bytes(puncs_en[flag])
             << "' with '" << converter.to_bytes(puncs_zh[flag])
             << "' at " << pos << " of blk " << blk_i << "." << endl;
      }
    }
  }

  void space(Wiki::Blocks &blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] : std::views::enumerate(blocks))
    {
      if (blk.type != WikiParser::TEXT)
        continue;

      ustring &str = blk.value;
      for (const auto &&[pos, c] : std::views::enumerate(str))
      {
        if (c != ' ')
          continue;

        bool front = pos != 0 && isalnum(str.at(pos - 1));
        bool front_sym = pos != 0 && issymbol(str.at(pos - 1));
        bool back = pos != str.size() - 1 && isalnum(str.at(pos + 1));
        bool back_sym = pos != str.size() - 1 && issymbol(str.at(pos + 1));

        if (front_sym || back_sym)
          continue;

        if (pos != 0 && pos != str.size() - 1 && front && back)
          continue;

        str.erase(pos, 1);
        fix_count++;
        cerr << "Remove space at " << pos << " of blk " << blk_i << "." << endl;
      }
    }
  }

  void newline(Wiki::Blocks blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] : std::views::enumerate(blocks))
    {
      if (blk.type != WikiParser::TEXT)
        continue;

      ustring &str = blk.value;
      for (const auto &&[pos, c] : std::views::enumerate(str))
      {
      }
    }
  }

  bool isdigit(char32_t ch)
  {
    return ((ch - '0' >= 0) && (ch - '0' < 10));
  }
  bool isalnum(char32_t ch)
  {
    return (ch - 'a' >= 0) && (ch - 'a' < 26) || (ch - 'A' >= 0) && (ch - 'A' < 26) || (ch - '0' >= 0) && (ch - '0' < 10);
  }
  bool issymbol(char32_t ch)
  {
    return (ch >= '!') && (ch <= '/') || (ch >= ':') && (ch <= '@') || (ch >= '[') && (ch <= '`') || (ch >= '{') && (ch <= '}');
  }
}
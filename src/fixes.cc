#include <set>
#include <iostream>
#include <algorithm>

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

  void footnotes(Wiki::UBlocks &blocks, int &fix_count)
  {
    for (auto it = blocks.begin(); it < std::prev(blocks.end()); it++)
    {
      Wiki::UBlock &block = *it;
      const size_t block_i = std::distance(blocks.begin(), it);

      if (block.type != WikiParser::TEXT)
        continue;

      const static ustring puncs = U"，。、！？；";
      const size_t punc_i = puncs.find(block.value.back());
      if (punc_i == string::npos)
        continue;

      Wiki::UBlock &next_block = *(&block + 1);
      if (next_block.type != WikiParser::HTML_TAG && next_block.type != WikiParser::HTML_SELF_CLOSING_TAG || !next_block.value.starts_with(U"ref"))
        continue;

      const auto next_text_it = std::find_if(it + 1, blocks.end(), [](Wiki::UBlock &block)
                                             { return block.type == WikiParser::TEXT || block.type == WikiParser::LINK; });
      // We're on last text block.
      if (next_text_it == blocks.end())
        break;

      Wiki::UBlock &next_text_block = *next_text_it;
      const size_t next_text_block_i = std::distance(blocks.begin(), next_text_it);

      // Find stop marker.
      const std::set<ustring> stop_markers{
          U"\n",
          U"”",
          U"」",
          U"』",
      };
      const auto next_stop_it = std::find_if(stop_markers.begin(), stop_markers.end(), [&block = next_text_block](ustring stop)
                                             { return block.value.starts_with(stop); });
      if (next_stop_it != stop_markers.end())
        continue;

      // Remove period.
      block.value.pop_back();
      fix_count++;

      const char32_t punc_ch = puncs.at(punc_i);

      if (puncs.find(next_text_block.value.front()) != string::npos)
      {
        cerr << "Remove '" << converter.to_bytes(punc_ch) << "' in block " << block_i << "." << endl;
        continue;
      }
      next_text_block.prepend = punc_ch;
      cerr << "Shift '" << converter.to_bytes(punc_ch)
           << "' from block " << block_i << " to " << next_text_block_i << "." << endl;
    }
  }

  void punctuation(Wiki::UBlocks &blocks, int &fix_count)
  {
    int flag;
    for (auto it = blocks.begin(); it < blocks.end(); it++)
    {
      Wiki::UBlock &block = *it;
      const size_t block_i = std::distance(blocks.begin(), it);

      if (block.type != WikiParser::TEXT || block.type != WikiParser::LINK)
        continue;

      const static ustring puncs = U"，。、“”【】！？『』〖〗：；「」・｜（）";

      ustring &str = block.value;
      for (auto str_it = str.begin(); str_it < str.end(); str_it++)
      {
        const char32_t &c = *str_it;
        size_t pos = std::distance(str.begin(), str_it);

        int flag_new = puncs.find(c);
        if (flag != string::npos && flag == flag_new)
        {
          str.erase(pos, 1);
          fix_count++;
          cerr << "Remove '" << converter.to_bytes(puncs[flag])
               << "' in block " << block_i << " pos " << pos << "." << endl;
        }
        flag = flag_new;
      }
    }
  }

  void punctuation_width(Wiki::UBlocks &blocks, int &fix_count)
  {
    for (auto it = blocks.begin(); it < blocks.end(); it++)
    {
      Wiki::UBlock &block = *it;
      const size_t block_i = std::distance(blocks.begin(), it);

      if (block.type != WikiParser::TEXT)
        continue;

      const static ustring puncs_en = U".,()＋．　";
      const static ustring puncs_zh = U"。，（）+. ";

      ustring &str = block.value;
      for (auto str_it = str.begin(); str_it < str.end(); str_it++)
      {
        const char32_t &c = *str_it;
        size_t pos = std::distance(str.begin(), str_it);

        int flag = puncs_en.find(c);
        if (flag == string::npos)
          continue;

        bool front = str_it != str.begin() && isalnum(str.at(pos - 1));
        bool front_num = str_it != str.begin() && isdigit(str.at(pos - 1));
        bool back = str_it != std::prev(str.end()) && isalnum(str.at(pos + 1));

        char32_t punc = puncs_zh.at(flag);

        if (punc != U'+' && punc != U'.' && punc != U' ' && front && back)
          continue;

        if (front_num && punc == U'。')
          continue;

        str[pos] = punc;
        fix_count++;
        cerr << "Replace '" << converter.to_bytes(puncs_en[flag])
             << "' with '" << converter.to_bytes(puncs_zh[flag])
             << "' in block " << block_i << " pos " << pos << "." << endl;
      }
    }
  }

  void space(Wiki::UBlocks &blocks, int &fix_count)
  {
    for (auto it = blocks.begin(); it < blocks.end(); it++)
    {
      Wiki::UBlock &block = *it;
      const size_t block_i = std::distance(blocks.begin(), it);

      if (block.type != WikiParser::TEXT)
        continue;

      ustring &str = block.value;
      for (auto str_it = str.begin(); str_it < str.end(); str_it++)
      {
        const char32_t &c = *str_it;
        size_t pos = std::distance(str.begin(), str_it);

        if (c != ' ')
          continue;

        bool first = str_it == str.begin();
        bool front = !first && isalnum(str.at(pos - 1));
        bool front_sym = !first && issymbol(str.at(pos - 1));
        bool last = str_it == std::prev(str.end());
        bool back = !last && isalnum(str.at(pos + 1));
        bool back_sym = !last && issymbol(str.at(pos + 1));

        if (front_sym || back_sym)
          continue;

        if (!first && !last && front && back)
          continue;

        str.erase(pos, 1);
        fix_count++;
        cerr << "Remove space in block " << block_i << " pos " << pos << "." << endl;
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
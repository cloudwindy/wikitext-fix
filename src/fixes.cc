#include <set>
#include <iostream>
#include <algorithm>

#include "utf-32.hh"
#include "fixes.hh"

using std::cout, std::cerr, std::endl;
using std::string;
using ustring = std::u32string;

const ustring puncs = U"，。、“”【】！？『』〖〗：；「」・｜（）";
const ustring puncs_single = U"，。、！？";
const std::set<ustring> stop_markers{
    U"\n",
    U"”",
    U"」",
    U"』",
};

namespace Fixes
{
  void footnotes(Wiki::UBlocks &blocks, int &fix_count)
  {
    for (auto it = blocks.begin(); it != std::prev(blocks.end()); it++)
    {
      Wiki::UBlock &block = *it;
      const size_t block_i = std::distance(blocks.begin(), it);

      const size_t punc_i = puncs_single.find(block.value.back());

      if (block.type != WikiParser::TEXT || punc_i == string::npos)
        continue;

      Wiki::UBlock &next_block = *(&block + 1);
      if (next_block.type != WikiParser::HTML_TAG || !next_block.value.starts_with(U"ref"))
        continue;

      const auto next_text_it = std::find_if(it + 1, blocks.end(), [](Wiki::UBlock &block)
                                             { return block.type == WikiParser::TEXT || block.type == WikiParser::LINK; });
      // We're on last text block.
      if (next_text_it == blocks.end())
        break;

      Wiki::UBlock &next_text_block = *next_text_it;
      const size_t next_text_block_i = std::distance(blocks.begin(), next_text_it);

      // Find stop marker.
      const auto next_stop_it = std::find_if(stop_markers.begin(), stop_markers.end(), [&block = next_text_block](ustring stop)
                                             { return block.value.starts_with(stop); });
      if (next_stop_it != stop_markers.end())
        continue;

      // Remove period.
      block.value.pop_back();
      fix_count++;

      const char32_t punc_ch = puncs_single.at(punc_i);

      if (puncs_single.find(next_text_block.value.front()) != string::npos)
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
    for (auto it = blocks.begin(); it != blocks.end(); it++)
    {
      Wiki::UBlock &block = *it;
      size_t block_i = std::distance(blocks.begin(), it);

      if (block.type != WikiParser::TEXT || block.type != WikiParser::LINK)
        continue;

      ustring &str = block.value;
      for (const auto &c : str)
      {
        size_t pos = &str.front() - &c;
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

  void space(Wiki::UBlocks &blocks, int &fix_count)
  {
  }
}
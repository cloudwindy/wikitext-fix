#include "fixes_internal.hh"

namespace Fixes
{
  void footnotes(Wiki::Blocks &blocks, int &fix_count)
  {
    for (size_t blk_i = 0; blk_i < blocks.size() - 1; blk_i++)
    {
      Wiki::Block &blk = blocks[blk_i], &blk_next = blocks[blk_i + 1];
      if (!is_blk_text(blk) || !is_blk_ref(blk_next))
        continue;

      const static ustring puncs = U"，。、！？；";
      const size_t punc_i = puncs.find(blk.value.back());
      if (punc_i == string::npos)
        continue;

      const auto tblk_next_it = std::ranges::find_if(blocks | drop(blk_i + 1), is_blk_visible);
      // Check if there isn't text block anymore.
      if (tblk_next_it == blocks.end())
        break;
      Wiki::Block &tblk_next = *tblk_next_it;
      const size_t tblk_next_i = std::distance(blocks.begin(), tblk_next_it);

      // Find stop marker.
      if (ustring(U"\n”」』").find(tblk_next.value.front()) != string::npos)
        continue;

      // Remove period.
      blk.value.pop_back();
      fix_count++;

      const char32_t punc_ch = puncs.at(punc_i);
      if (puncs.find(tblk_next.value.front()) != string::npos)
      {
        cerr << "Remove '" << converter.to_bytes(punc_ch)
             << "' in blk " << blk_i << "." << endl;
        continue;
      }
      blocks.insert(tblk_next_it, {.type = WikiParser::TEXT, .value = ustring(1, punc_ch)});
      tblk_next.value.insert(0, 1, punc_ch);
      cerr << "Shift '" << converter.to_bytes(punc_ch)
           << "' from blk " << blk_i
           << " to blk " << tblk_next_i << "." << endl;
    }
  }
};
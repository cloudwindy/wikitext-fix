#include "fixes_internal.hh"

namespace Fixes
{
  void newline(Wiki::Blocks blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] :
         blocks | filter(is_blk_text) | enumerate)
    {
      ustring &str = blk.value;
      for (const auto &&[pos, c] :
           str | filter(is_ln) | enumerate)
      {
        if (c != U'\n')
          continue;
      }
    }
  }
};
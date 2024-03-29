#include "fixes_internal.hh"

using namespace Fixes;

void Fix::newline()
{
  for (const auto &&[blk_i, blk] :
       blocks | filter(is_blk_text) | enumerate)
  {
    ustring &str = blk.value;
    for (size_t pos = 0; pos < str.size(); pos++)
    {
      if (!is_ln(str[pos]))
        continue;

      constexpr auto block_markers = U"*#;:"sv;
    }
  }
}
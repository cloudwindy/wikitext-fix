#include "fixes_internal.hh"

namespace Fixes
{
  void punctuation(Wiki::Blocks &blocks, int &fix_count)
  {
    int flag;
    for (const auto &&[blk_i, blk] :
         blocks | filter(is_blk_visible) | enumerate)
    {
      constexpr auto puncs = U"，。、“”【】！？『』〖〗：；「」・｜（）"sv;

      ustring &str = blk.value;
      for (size_t pos = 0; pos < str.size(); pos++)
      {
        int flag_new = puncs.find(str[pos]);
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
};
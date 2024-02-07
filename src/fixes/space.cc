#include "fixes_internal.hh"

namespace Fixes
{
  void space_unnecessary(Wiki::Blocks &blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] :
         blocks | filter(is_blk_text) | enumerate)
    {
      ustring &str = blk.value;
      for (size_t pos = 0; pos < str.size(); pos++)
      {
        if (!is_space(str[pos]))
          continue;

        bool front = pos != 0 && is_alnum(str.at(pos - 1));
        bool front_sym = pos != 0 && is_symbol(str.at(pos - 1));
        bool back = pos != str.size() - 1 && is_alnum(str.at(pos + 1));
        bool back_sym = pos != str.size() - 1 && is_symbol(str.at(pos + 1));

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
};
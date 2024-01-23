#include "fixes_internal.hh"

namespace Fixes
{
  void punctuation_width(Wiki::Blocks &blocks, int &fix_count)
  {
    for (const auto &&[blk_i, blk] :
         blocks | filter(is_blk_text) | enumerate)
    {
      ustring &str = blk.value;
      for (const auto &&[pos, c] : str | enumerate)
      {
        const static ustring puncs_en = U".,()＋．　丶";
        const static ustring puncs_zh = U"。，（）+. 、";
        int flag = puncs_en.find(c);
        if (flag == string::npos)
          continue;

        bool front = pos != 0 && is_alnum(str.at(pos - 1));
        bool back = pos != str.size() - 1 && is_alnum(str.at(pos + 1));

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
};
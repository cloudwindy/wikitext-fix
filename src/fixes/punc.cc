#include "fixes_internal.hh"

using namespace Fixes;

void Fix::punc_duplicate()
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
        count++;
        cerr << "Remove '" << converter.to_bytes(puncs[flag])
             << "' at " << pos << " of blk " << blk_i << "." << endl;
      }
      flag = flag_new;
    }
  }
}
void Fix::punc_wrong_width()
{
  for (const auto &&[blk_i, blk] :
       blocks | filter(is_blk_text) | enumerate)
  {
    ustring &str = blk.value;
    for (const auto &&[pos, c] : str | enumerate)
    {
      constexpr auto puncs_en = U".,()＋．　丶"sv;
      constexpr auto puncs_zh = U"。，（）+. 、"sv;
      size_t flag = puncs_en.find(c);
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
      count++;
      cerr << "Replace '" << converter.to_bytes(puncs_en[flag])
           << "' with '" << converter.to_bytes(puncs_zh[flag])
           << "' at " << pos << " of blk " << blk_i << "." << endl;
    }
  }
}
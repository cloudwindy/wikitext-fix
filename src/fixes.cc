#include <iostream>

#include "utf-32.hh"
#include "fixes.hh"

using std::cout, std::cerr, std::endl;
using std::string;
using ustring = std::u32string;

namespace Fixes
{
  void footnotes(ustring &str)
  {
    size_t pos = 0;
    while (true)
    {
      pos = str.find(U"。<ref", pos);
      if (pos == string::npos)
        break;

      size_t pos_a = str.find(U"</ref>", pos);
      size_t pos_b = str.find(U"/>", pos);
      size_t pos_line = str.find(U"\n", pos);
      if (pos_a + 6 == pos_line || pos_b + 2 == pos_line)
      {
        pos++;
        continue;
      }
      str.erase(pos, 1);

      size_t pos_new;
      if (pos_a > pos_b)
        pos_new = pos_b + 2 - 1;
      else if (pos_a < pos_b)
        pos_new = pos_a + 6 - 1;
      else if (pos_a == string::npos)
        break;

      if (str[pos_new] == U'。')
      {
        cerr << "Removal of '。' in " << pos << endl;
        continue;
      }
      str.insert(pos_new, U"。");
      cerr << "Shift of '。' from " << pos << " to " << pos_new << endl;
    }
  }

  void punctuation(ustring &str)
  {
    const static ustring puncs = U"，。、“”【】！？『』〖〗：；「」・｜（）";
    int flag;
    for (auto it = str.begin(); it != str.end();)
    {
      size_t i = it - str.begin();
      int flag_new = puncs.find(str[i]);
      if (flag_new != string::npos && flag == flag_new)
      {
        str.erase(i, 1);
        cerr << "Removal of '" << converter.to_bytes(puncs[flag]) << "' in " << i << endl;
      }
      else
      {
        flag = flag_new;
        it++;
      }
    }
  }

  void space(ustring &str)
  {
    
  }
}
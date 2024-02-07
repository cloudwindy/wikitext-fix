#pragma once

#include "wikitext.hh"

namespace Fixes
{
  void footnote_position(Wiki::Blocks &str, int &fix_count);
  void punc_duplicate(Wiki::Blocks &str, int &fix_count);
  void punc_wrong_width(Wiki::Blocks &str, int &fix_count);
  
  void space_unnecessary(Wiki::Blocks &str, int &fix_count);
}

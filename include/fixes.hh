#pragma once

#include <string>

#include "wikitext.hh"

namespace Fixes
{
  void footnotes(Wiki::Blocks &str, int &fix_count);
  void punctuation(Wiki::Blocks &str, int &fix_count);
  void punctuation_width(Wiki::Blocks &str, int &fix_count);
  void space(Wiki::Blocks &str, int &fix_count);
}

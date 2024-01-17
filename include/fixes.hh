#pragma once

#include <string>

#include "wikitext.hh"

namespace Fixes
{
  void footnotes(Wiki::UBlocks &str, int &fix_count);
  void punctuation(Wiki::UBlocks &str, int &fix_count);
  void punctuation_width(Wiki::UBlocks &str, int &fix_count);
  void space(Wiki::UBlocks &str, int &fix_count);
}

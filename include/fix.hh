#pragma once

#include "wikitext.hh"

class Fix
{
public:
  Fix(Wiki::Blocks &blocks) : blocks(blocks){};
  void newline();
  void footnote_position();
  void footnote_logical_position();
  void punc_duplicate();
  void punc_wrong_width();
  void space_unnecessary();

  int count = 0;

private:
  Wiki::Blocks &blocks;
};

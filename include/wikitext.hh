#pragma once

#include <vector>
#include <string>

namespace WikitextParser
{
  void print(std::string wt);
  enum BlockType
  {
    TEXT,
    TEMPLATE,
    TABLE,
    LINK,
    EXT_LINK,
    COMMENT,
    HTML_TAG,              // <ref>
    HTML_CLOSE_TAG,        // </ref>
    HTML_SELF_CLOSING_TAG, // <ref />
  };
  struct Block
  {
    BlockType type;
    std::string value;
  };
  enum Token
  {
    CHAR,
    TEMPLATE_BEGIN,
    TEMPLATE_END,
    TABLE_BEGIN,
    TABLE_END,
    LINK_BEGIN,
    LINK_END,
    EXT_LINK_BEGIN,
    EXT_LINK_END,
    COMMENT_BEGIN,
    COMMENT_END,
    HTML_TAG_BEGIN,       // <
    HTML_TAG_END,         // >
    HTML_CLOSE_TAG_BEGIN, // </
    HTML_CLOSE_TAG_END,   // />
  };
  class Parser
  {
  public:
    std::vector<Block> blocks;
    Parser(std::string wikitext);
    void parse();
    void push(BlockType type, std::string value);
    void push_text(std::string text);
    Token next();

  private:
    std::string wt;
  };
}

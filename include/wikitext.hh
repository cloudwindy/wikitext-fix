#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace WikiParser
{
  enum BlockType
  {
    TEXT,
    TEMPLATE,              // {{Citation needed}}
    TABLE,                 // {| ! Title |- | content |}
    LINK,                  // [[Wikipedia]]
    EXT_LINK,              // [Google https://google.com]
    COMMENT,               // <!--comment-->
    CONV_TAG,              // -{zh:内存;zh-tw:記憶體}-
    HTML_TAG,              // <ref>
    HTML_BODY,             //
    HTML_CLOSE_TAG,        // </ref>
    HTML_SELF_CLOSING_TAG, // <ref />
  };
  struct Block
  {
    BlockType type;
    std::string value;
  };
  using Blocks = std::vector<Block>;
  class BlockParser
  {
  public:
    enum Token : int;
    BlockParser(std::string wt);
    void set_wikitext(std::string wt);
    void reset();
    Blocks get_blocks() const;
    void parse();

  private:
    Token next_token();
    void template_begin();
    void template_end();
    void table_begin();
    void table_end();
    void link_begin();
    void link_end();
    void ext_link_begin();
    void ext_link_end();
    void comment_begin();
    void comment_end();
    void conv_tag_begin();
    void conv_tag_end();
    void html_tag_begin();
    void html_tag_end();
    void html_close_tag_begin();
    void html_close_tag_end();
    void make_block(BlockType type, bool allow_empty = false);
    void update_status();
    void update_buffer();
    struct state
    {
      unsigned int template_level : 4;
      unsigned int link_level : 4;
      unsigned int html_level : 4;
      unsigned int in_table : 1;
      unsigned int in_comment : 1;
      unsigned int in_ext_link : 1;
      unsigned int in_conv_tag : 1;
      unsigned int in_html_tag : 1;
      unsigned int in_html_close_tag : 1;
      unsigned int literal : 1;
    };
    state s = {0};
    std::string wt;
    std::string buf;
    Blocks blocks;
  };
};

namespace Wiki
{
  using WikiParser::BlockType;
  struct Block
  {
    BlockType type;
    std::u32string value;
  };
  using Blocks = std::vector<Block>;
  class Wikitext
  {
  public:
    Wikitext(Blocks ublocks);
    Wikitext(std::string wikitext);
    std::string to_string() const;
    std::string color_html() const;
    Blocks decode();
    size_t size();
    friend std::ostream &operator<<(std::ostream &os, const Wikitext &s);

  private:
    WikiParser::Blocks parser_blocks;
  };
};

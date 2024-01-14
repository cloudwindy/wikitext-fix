#include <vector>
#include <iostream>
#include "wikitext.hh"
#include "utf-32.hh"

using std::vector, std::string;
#define MATCH_BEGIN(target, pattern, token) \
  if (target.starts_with(pattern))          \
  {                                         \
    target.erase(0, sizeof(pattern) - 1);   \
    return token;                           \
  }
#define NEXT(target, pattern, token) MATCH_BEGIN(target, pattern, token)
#define MATCH_END(token) return token

namespace WikitextParser
{
  void print(string wt)
  {
    Parser parser(wt);
    std::cout << "Parser started" << std::endl;
    parser.parse();
    std::cout << "Parsed block count=" << parser.blocks.size() << std::endl;
    for (auto block : parser.blocks)
    {
      if (block.type == LINK)
        std::cout << block.value << std::endl;
    }
  }
  Parser::Parser(string wt)
  {
    this->wt = wt;
  }
  void Parser::parse()
  {
    string text;
    struct state
    {
      bool in_template : 1;
      bool in_table : 1;
      bool in_comment : 1;
      bool in_link : 1;
      bool in_ext_link : 1;
    };
    state s;
    while (!wt.empty())
    {
      bool literal = s.in_template || s.in_table || s.in_comment || s.in_link || s.in_ext_link;
      Token t = next();
      switch (t)
      {
      case CHAR:
        text.push_back(wt[0]);
        wt.erase(0, 1);
        break;
      case TEMPLATE_BEGIN:
        if (literal)
        {
          text += "{{";
          break;
        }
        s.in_template = true;
        push_text(text);
        text.clear();
        break;
      case TEMPLATE_END:
        if (!s.in_template)
        {
          text += "}}";
          break;
        }
        s.in_template = false;
        push(TEMPLATE, text);
        text.clear();
        break;
      case TABLE_BEGIN:
        if (literal)
        {
          text += "{|";
          break;
        }
        s.in_table = true;
        push_text(text);
        text.clear();
        break;
      case TABLE_END:
        if (!s.in_table)
        {
          text += "|}";
          break;
        }
        s.in_table = false;
        push(TABLE, text);
        text.clear();
        break;
      case LINK_BEGIN:
        if (literal)
        {
          text += "[[";
          break;
        }
        s.in_link = true;
        push_text(text);
        text.clear();
        break;
      case LINK_END:
        if (!s.in_link)
        {
          text += "]]";
          break;
        }
        s.in_link = false;
        push(LINK, text);
        text.clear();
        break;
      case EXT_LINK_BEGIN:
        if (literal)
        {
          text += "[";
          break;
        }
        s.in_ext_link = true;
        push_text(text);
        text.clear();
        break;
      case EXT_LINK_END:
        if (!s.in_ext_link)
        {
          text += "]";
          break;
        }
        s.in_ext_link = false;
        push(EXT_LINK, text);
        text.clear();
        break;
      case COMMENT_BEGIN:
        if (literal)
        {
          text += "<!--";
          break;
        }
        s.in_comment = true;
        push_text(text);
        text.clear();
        break;
      case COMMENT_END:
        if (!s.in_comment)
        {
          text += "-->";
          break;
        }
        s.in_comment = false;
        push(COMMENT, text);
        text.clear();
        break;
      }
    }
    push_text(text);
  }
  void Parser::push(BlockType type, string value)
  {
    blocks.push_back({.type = type, .value = value});
  }
  void Parser::push_text(string text)
  {
    if (!text.empty())
    {
      blocks.push_back({.type = TEXT, .value = text});
    }
  }
  Token Parser::next()
  {
    MATCH_BEGIN(wt, "{{", TEMPLATE_BEGIN);
    NEXT(wt, "}}", TEMPLATE_END);
    NEXT(wt, "{|", TABLE_BEGIN);
    NEXT(wt, "|}", TABLE_END);
    NEXT(wt, "[[", LINK_BEGIN);
    NEXT(wt, "]]", LINK_END);
    NEXT(wt, "[", EXT_LINK_BEGIN);
    NEXT(wt, "]", EXT_LINK_END);
    NEXT(wt, "<!--", COMMENT_BEGIN);
    NEXT(wt, "-->", COMMENT_END);
    NEXT(wt, "<", HTML_TAG_BEGIN);
    NEXT(wt, ">", HTML_TAG_END);
    NEXT(wt, "</", HTML_CLOSE_TAG_BEGIN);
    NEXT(wt, "/>", HTML_CLOSE_TAG_END);
    MATCH_END(CHAR);
  }
}
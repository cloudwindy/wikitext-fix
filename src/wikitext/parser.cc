#include <array>
#include <algorithm>
#include <set>
#include <cstring>

#include "wikitext.hh"

using std::string;
#define MATCH_BEGIN(target, view, pattern, token)        \
  if (!std::strncmp(view, pattern, sizeof(pattern) - 1)) \
  {                                                      \
    target.erase(0, sizeof(pattern) - 1);                \
    return token;                                        \
  }
#define NEXT(target, view, pattern, token) MATCH_BEGIN(target, view, pattern, token)
#define MATCH_END(token) return token

namespace WikiParser
{
  using namespace std::string_view_literals;

  enum BlockParser::Token : int
  {
    CHAR,
    BOLD_STYLE_MARKER,    // '''
    ITALIC_STYLE_MARKER,  // ''
    TEMPLATE_BEGIN,       // {{
    TEMPLATE_END,         // }}
    TABLE_BEGIN,          // {|
    TABLE_END,            // |}
    LINK_BEGIN,           // [[
    LINK_END,             // ]]
    EXT_LINK_BEGIN,       // [
    EXT_LINK_END,         // ]
    COMMENT_BEGIN,        // <!--
    COMMENT_END,          // -->
    CONV_TAG_BEGIN,       // -{
    CONV_TAG_END,         // }-
    HTML_CLOSE_TAG_BEGIN, // </
    HTML_CLOSE_TAG_END,   // />
    HTML_TAG_BEGIN,       // <
    HTML_TAG_END,         // >
  };

  BlockParser::BlockParser(string wt) : wt(wt){};
  void BlockParser::set_wikitext(string wt)
  {
    this->wt = wt;
  }
  void BlockParser::reset()
  {
    s = {0};
    wt.clear();
    blocks.clear();
  }
  Blocks BlockParser::get_blocks() const
  {
    return blocks;
  }
  void BlockParser::parse()
  {
    while (!wt.empty())
    {
      update_status();
      update_buffer();

      if (wt.empty())
      {
        // Don't forget last text block.
        make_text_block();
        break;
      }

      switch (next_token())
      {
      case CHAR:
        buf.push_back(wt.at(0));
        wt.erase(0, 1);
        break;
      case ITALIC_STYLE_MARKER:
        if (!s.literal)
        {
          make_text_block();
          s.style_italic = ~s.style_italic;
        }
        else
        {
          buf.append("''");
        }
        break;
      case BOLD_STYLE_MARKER:
        if (!s.literal)
        {
          make_text_block();
          s.style_bold = ~s.style_bold;
        }
        else
        {
          buf.append("'''");
        }
        break;
      case TEMPLATE_BEGIN:
        template_begin();
        break;
      case TEMPLATE_END:
        template_end();
        break;
      case TABLE_BEGIN:
        table_begin();
        break;
      case TABLE_END:
        table_end();
        break;
      case LINK_BEGIN:
        link_begin();
        break;
      case LINK_END:
        link_end();
        break;
      case EXT_LINK_BEGIN:
        ext_link_begin();
        break;
      case EXT_LINK_END:
        ext_link_end();
        break;
      case COMMENT_BEGIN:
        comment_begin();
        break;
      case COMMENT_END:
        comment_end();
        break;
      case CONV_TAG_BEGIN:
        conv_tag_begin();
        break;
      case CONV_TAG_END:
        conv_tag_end();
        break;
      case HTML_TAG_BEGIN:
        html_tag_begin();
        break;
      case HTML_TAG_END:
        html_tag_end();
        break;
      case HTML_CLOSE_TAG_BEGIN:
        html_close_tag_begin();
        break;
      case HTML_CLOSE_TAG_END:
        html_close_tag_end();
        break;
      }
    }
  }
  // {{
  void BlockParser::template_begin()
  {
    if (s.literal)
    {
      buf.append("{{");
      if (s.template_level)
        s.template_level++;
      return;
    }
    make_text_block();
    s.template_level++;
  }
  // }}
  void BlockParser::template_end()
  {
    if (s.template_level != 1)
    {
      buf.append("}}");
      if (s.template_level)
        s.template_level--;
      return;
    }
    s.template_level--;
    make_block(TEMPLATE);
  }
  // {|
  void BlockParser::table_begin()
  {
    if (s.literal)
    {
      buf.append("{|");
      return;
    }
    make_text_block();
    s.in_table = true;
  }
  // |}
  void BlockParser::table_end()
  {
    if (!s.in_table)
    {
      if (wt.at(0) == '}')
      {
        // Actually a template end.
        buf.append("|");
        wt.erase(0, 1);
        template_end();
      }
      else
      {
        buf.append("|}");
      }
      return;
    }
    s.in_table = false;
    make_block(TABLE);
  }
  // [[
  void BlockParser::link_begin()
  {
    if (s.literal)
    {
      buf.append("[[");
      if (s.link_level)
        s.link_level++;
      return;
    }
    make_text_block();
    s.link_level++;
  }
  // ]]
  void BlockParser::link_end()
  {
    if (s.link_level != 1)
    {
      buf.append("]]");
      if (s.link_level)
        s.link_level--;
      return;
    }
    s.link_level--;
    make_block(LINK);
  }
  // [
  void BlockParser::ext_link_begin()
  {
    if (s.literal)
    {
      buf.append("[");
      return;
    }
    make_text_block();
    s.in_ext_link = true;
  }
  // ]
  void BlockParser::ext_link_end()
  {
    if (!s.in_ext_link)
    {
      buf.append("]");
      return;
    }
    s.in_ext_link = false;
    make_block(EXT_LINK);
  }
  // <!--
  void BlockParser::comment_begin()
  {
    if (s.literal)
    {
      buf.append("<!--");
      return;
    }
    make_text_block();
    s.in_comment = true;
  }
  // -->
  void BlockParser::comment_end()
  {
    if (!s.in_comment)
    {
      buf.append("-->");
      return;
    }
    s.in_comment = false;
    make_block(COMMENT);
  }
  // -{
  void BlockParser::conv_tag_begin()
  {
    if (s.literal)
    {
      buf.append("-{");
      return;
    }
    make_text_block();
    s.in_conv_tag = true;
  }
  // }-
  void BlockParser::conv_tag_end()
  {
    if (!s.in_conv_tag)
    {
      buf.append("}-");
      return;
    }
    s.in_conv_tag = false;
    make_block(CONV_TAG, TextStyle::NONE, true);
  }
  // <tag
  // ^
  void BlockParser::html_tag_begin()
  {
    if (s.literal || !std::isalpha(wt.front()))
    {
      buf.append("<");
      if (s.html_level && std::isalpha(wt.front()))
        s.in_html_tag = true;
      return;
    }
    make_text_block();
    s.in_html_tag = true;
  }
  // tag>
  //    ^
  void BlockParser::html_tag_end()
  {
    if (s.html_level || !s.in_html_tag && !s.in_html_close_tag)
      buf.append(">");

    // <tag>
    //     ^
    if (s.in_html_tag)
    {
      // HTML void elements: https://html.spec.whatwg.org/multipage/syntax.html#void-elements
      constexpr std::array void_elements{
          "area"sv, "base"sv, "br"sv, "col"sv, "embed"sv, "hr"sv,
          "img"sv, "input"sv, "link"sv, "meta"sv, "source"sv, "track"sv,
          "wbr"sv};
      const auto it = std::ranges::find_if(
          void_elements,
          [buf = buf](std::string_view element)
          { return buf.starts_with(element); });
      if (!s.html_level)
        make_block(HTML_TAG);

      if (it == void_elements.end())
        s.html_level++;

      s.in_html_tag = false;
    }
    // </tag>
    //      ^
    else if (s.in_html_close_tag)
    {
      if (!s.html_level)
        make_block(HTML_CLOSE_TAG);
      s.in_html_close_tag = false;
    }
  }
  // </tag>
  //  ^
  void BlockParser::html_close_tag_begin()
  {
    if (s.html_level != 1)
    {
      buf.append("</");
      if (s.html_level)
      {
        s.in_html_close_tag = true;
        s.html_level--;
      }
      return;
    }
    s.in_html_close_tag = true;
    s.html_level--;
    make_block(HTML_BODY);
  }
  // <tag />
  //       ^
  void BlockParser::html_close_tag_end()
  {
    if (!s.in_html_tag || s.html_level)
    {
      s.in_html_tag = false;
      buf.append("/>");
      return;
    }
    s.in_html_tag = false;
    make_block(HTML_SELF_CLOSING_TAG);
  }
  void BlockParser::make_text_block()
  {
    TextStyle style = TextStyle::NONE;
    if (s.style_bold && s.style_italic)
      style = TextStyle::ITALIC_BOLD;
    else if (s.style_bold)
      style = TextStyle::BOLD;
    else if (s.style_italic)
      style = TextStyle::ITALIC;
    make_block(TEXT, style);
  }
  void BlockParser::make_block(BlockType type, TextStyle style, bool allow_empty)
  {
    update_status();
    // Certain conditions must be met when making a block:
    // 1. Buffer isn't empty unless allow_empty.
    // 2. Not literal unless not a TEXT block.
    if ((!buf.empty() || allow_empty) && (!s.literal || type != TEXT))
    {
      blocks.push_back({.type = type, .style = style, .value = std::move(buf)});
    }
  }
  void BlockParser::update_status()
  {
    s.literal = s.template_level ||
                s.link_level ||
                s.html_level ||
                s.in_table ||
                s.in_comment ||
                s.in_ext_link ||
                s.in_conv_tag ||
                s.in_html_tag ||
                s.in_html_close_tag;
  }
  void BlockParser::update_buffer()
  {
    size_t text_len = 0;
    for (const char &ch : wt)
    {
      if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == '|' || ch == '<' || ch == '>' || ch == '/' || ch == '-' || ch == '\'' || ch == '!')
        break;
      text_len++;
    }
    if (text_len)
    {
      buf.append(wt.substr(0, text_len));
      wt.erase(0, text_len);
    }
  }
  BlockParser::Token BlockParser::next_token()
  {
    const char *c = wt.c_str();
    MATCH_BEGIN(wt, c, "{{", TEMPLATE_BEGIN);
    NEXT(wt, c, "}}", TEMPLATE_END);
    NEXT(wt, c, "[[", LINK_BEGIN);
    NEXT(wt, c, "]]", LINK_END);
    NEXT(wt, c, "{|", TABLE_BEGIN);
    NEXT(wt, c, "|}", TABLE_END);
    NEXT(wt, c, "[", EXT_LINK_BEGIN);
    NEXT(wt, c, "]", EXT_LINK_END);
    NEXT(wt, c, "-{", CONV_TAG_BEGIN);
    NEXT(wt, c, "}-", CONV_TAG_END);
    NEXT(wt, c, "</", HTML_CLOSE_TAG_BEGIN);
    NEXT(wt, c, "/>", HTML_CLOSE_TAG_END);
    NEXT(wt, c, "<", HTML_TAG_BEGIN);
    NEXT(wt, c, ">", HTML_TAG_END);
    NEXT(wt, c, "'''", BOLD_STYLE_MARKER);
    NEXT(wt, c, "''", ITALIC_STYLE_MARKER);
    NEXT(wt, c, "<!--", COMMENT_BEGIN);
    NEXT(wt, c, "-->", COMMENT_END);
    MATCH_END(CHAR);
  }
}

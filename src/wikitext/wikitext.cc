#include <iostream>

#include "wikitext.hh"
#include "utf-32.hh"

using std::string;
using ustring = std::u32string;
using error = std::runtime_error;
using namespace WikiParser;

namespace Wiki
{
  string html_encode(string data);

  Wikitext::Wikitext(Blocks ublocks)
  {
    if (ublocks.empty())
      throw error("wikitext not available");

    parser_blocks.reserve(ublocks.size());
    for (const auto &ublock : ublocks)
      parser_blocks.push_back({.type = ublock.type,
                               .value = converter.to_bytes(ublock.value)});
  }
  Wikitext::Wikitext(string wikitext)
  {
    BlockParser parser(wikitext);
    parser.parse();
    parser_blocks = parser.get_blocks();
    if (parser_blocks.empty())
      throw error("wikitext not available");
  }
  Blocks Wikitext::decode()
  {
    Blocks ublocks;
    ublocks.reserve(parser_blocks.size());
    for (const auto &block : parser_blocks)
      ublocks.push_back({.type = block.type,
                         .value = converter.from_bytes(block.value)});
    return ublocks;
  }
  string Wikitext::to_string() const
  {
    if (parser_blocks.empty())
      throw error("wikitext not parsed");

    string buf;
    for (const auto &block : parser_blocks)
    {
      string begin;
      string end;
      switch (block.type)
      {
      case TEMPLATE:
        begin = "{{";
        end = "}}";
        break;
      case TABLE:
        begin = "{|";
        end = "|}";
        break;
      case LINK:
        begin = "[[";
        end = "]]";
        break;
      case EXT_LINK:
        begin = "[";
        end = "]";
        break;
      case COMMENT:
        begin = "<!--";
        end = "-->";
        break;
      case CONV_TAG:
        begin = "-{";
        end = "}-";
        break;
      case HTML_TAG:
        begin = "<";
        end = ">";
        break;
      case HTML_SELF_CLOSING_TAG:
        begin = "<";
        end = "/>";
        break;
      case HTML_CLOSE_TAG:
        begin = "</";
        end = ">";
        break;
      }
      buf += begin + block.value + end;
    }
    return buf;
  }
  string Wikitext::color_html() const
  {
    if (parser_blocks.empty())
      throw error("wikitext not parsed");

    string buf;
    for (const auto &block : parser_blocks)
    {
      string begin;
      string end;
      switch (block.type)
      {
      case TEMPLATE:
        begin = "<span class=\"tpl\">{{";
        end = "}}</span>";
        break;
      case TABLE:
        begin = "<span class=\"tab\">{|";
        end = "|}</span>";
        break;
      case LINK:
        begin = "<span class=\"ln\">[[";
        end = "]]</span>";
        break;
      case EXT_LINK:
        begin = "<span class=\"eln\">[";
        end = "]</span>";
        break;
      case COMMENT:
        begin = "<span class=\"cm\">&lt;!--";
        end = "--&gt;</span>";
        break;
      case CONV_TAG:
        begin = "<span class=\"co\">-{";
        end = "}-</span>";
        break;
      case HTML_TAG:
        begin = "<span class=\"tag\">&lt;";
        end = "></span>";
        break;
      case HTML_SELF_CLOSING_TAG:
        begin = "<span class=\"tag\">&lt;";
        end = "/&gt;</span>";
        break;
      case HTML_CLOSE_TAG:
        begin = "<span class=\"tag\">&lt;/";
        end = "&gt;</span>";
        break;
      case HTML_BODY:
        begin = "<span class=\"body\">";
        end = "</span>";
        break;
      }
      buf += begin + html_encode(block.value) + end;
    }
    const static string css = R"(
    body {
      white-space: pre-wrap;
      font-family: Noto Sans Mono CJK, monospace;
    }
    .tpl {
      background-color: #a1907d;
    }
    .ln {
      color: #0070E0;
    }
    .tag {
      color: #36528D;
    }
    .body {
      background-color: #dbe7ff;
    }
    )";
    return "<!DOCTYPE html><html lang=\"zh-cn\"><body>" + buf + "<style>" + css + "</style></body></html>";
  }
  size_t Wikitext::size()
  {
    return parser_blocks.size();
  }
  std::ostream &operator<<(std::ostream &os, const Wikitext &wikitext)
  {
    return (os << wikitext.to_string());
  }
};

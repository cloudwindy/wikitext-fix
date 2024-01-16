#include <iostream>

#include "wikitext.hh"
#include "utf-32.hh"

using std::string;
using ustring = std::u32string;
using namespace WikiParser;
string html_encode(string data);

namespace Wiki
{
  Wikitext::Wikitext(UBlocks ublocks)
  {
    blocks.reserve(ublocks.size());
    for (const auto &ublock : ublocks)
    {
      if (!ublock.prepend.empty())
        blocks.push_back({.type = WikiParser::TEXT,
                          .value = converter.to_bytes(ublock.prepend)});

      blocks.push_back({.type = ublock.type,
                        .value = converter.to_bytes(ublock.value)});

      if (!ublock.append.empty())
        blocks.push_back({.type = WikiParser::TEXT,
                          .value = converter.to_bytes(ublock.append)});
    }
  }
  Wikitext::Wikitext(string wikitext)
  {
    Parser parser(wikitext);
    parser.parse();
    blocks = parser.get_blocks();
  }
  UBlocks Wikitext::decode()
  {
    UBlocks ublocks;
    ublocks.reserve(blocks.size());
    for (const auto &block : blocks)
    {
      ublocks.push_back({.type = block.type,
                         .value = converter.from_bytes(block.value)});
    }
    return ublocks;
  }
  string Wikitext::to_string() const
  {
    string buf;
    for (const auto &block : blocks)
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
    string buf;
    for (const auto &block : blocks)
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
    return blocks.size();
  }
  std::ostream &operator<<(std::ostream &os, const Wikitext &wikitext)
  {
    return (os << wikitext.to_string());
  }
};

string html_encode(string data)
{
  string buffer;
  buffer.reserve(data.size());
  for (size_t pos = 0; pos != data.size(); ++pos)
  {
    switch (data[pos])
    {
    case '&':
      buffer.append("&amp;");
      break;
    case '\"':
      buffer.append("&quot;");
      break;
    case '\'':
      buffer.append("&apos;");
      break;
    case '<':
      buffer.append("&lt;");
      break;
    case '>':
      buffer.append("&gt;");
      break;
    default:
      buffer.append(&data[pos], 1);
      break;
    }
  }
  return buffer;
}
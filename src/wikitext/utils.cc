#include <string>

using std::string;

namespace Wiki
{
  string html_encode(string data)
  {
    string buffer;
    buffer.reserve(data.size());
    for (const char &ch : data)
    {
      switch (ch)
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
        buffer.append(&ch, 1);
        break;
      }
    }
    return buffer;
  }
}
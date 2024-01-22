#include <iostream>
#include <fstream>
#include <CLI/CLI.hpp>

#include "api.hh"
#include "wikitext.hh"
#include "fixes.hh"
#include "utf-32.hh"

using std::cout, std::cerr, std::endl;
using std::string;
using ustring = std::u32string;

class StreamType
{
public:
  StreamType &operator<<(const char *);
};

int main(int argc, char *argv[])
{
  CLI::App app;

  string page_name, output_path;
  int old_id = 0;
  bool render = false, no_fix = false;
  app.add_option("pos", page_name, "Page title name")->required();
  app.add_option("-o,--output", output_path, "Write to file instead of stdout");
  app.add_option("-O,--oldid", old_id, "Use an old version");
  app.add_flag("-r,--render", render, "Output parser rendered html");
  app.add_flag("-n,--no-fix", no_fix, "Disable fixing");
  app.validate_positionals();
  app.validate_optional_arguments();

  CLI11_PARSE(app, argc, argv);

  cerr << "Fetching " << page_name << "..." << endl;
  string bytes = MWAPI::page_wikitext(page_name);
  cerr << "Parsing..." << endl;
  Wiki::Wikitext wikitext(bytes);
  Wiki::Blocks ublocks = wikitext.decode();

  if (!no_fix)
  {
    cerr << "Fixing... (" << wikitext.size() << " blocks)" << endl;
    int fix_count = 0;
    for (int i = 0; i < 3; i++)
    {
      Fixes::space(ublocks, fix_count);
      Fixes::footnotes(ublocks, fix_count);
      Fixes::punctuation(ublocks, fix_count);
      Fixes::punctuation_width(ublocks, fix_count);
    }

    if (!fix_count)
    {
      cerr << "No need to fix." << endl;
    }
    else if (fix_count == 1)
    {
      cerr << "1 fix applied." << endl;
    }
    else if (fix_count > 1)
    {
      cerr << fix_count << " fixes applied." << endl;
    }
  }
  else
  {
    cerr << "Not fixed. (" << wikitext.size() << " blocks)" << endl;
  }

  wikitext = Wiki::Wikitext(ublocks);

  if (render)
    cout << wikitext.color_html() << endl;
  else
    cout << wikitext << endl;

  return EXIT_SUCCESS;
}

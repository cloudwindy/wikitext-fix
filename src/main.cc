#include <iostream>
#include <fstream>

#include <CLI/CLI.hpp>

#include "api.hh"
#include "wikitext.hh"
#include "fixes.hh"
#include "utf-32.hh"

using std::cout, std::cerr, std::endl, std::flush;
using std::string;
using ustring = std::u32string;

int main(int argc, char *argv[])
{
  CLI::App app;

  string page_name, output_path, login, password;
  int old_id = 0;
  bool edit = false, render = false, no_fix = false, silent = false;
  app.add_option("pos", page_name, "Page title name")->required();
  app.add_flag("-e,--edit", edit, "Edit page");
  app.add_flag("-n,--no-fix", no_fix, "Disable fixing");
  app.add_option("-o,--output", output_path, "Write to file instead of stdout");
  app.add_option("-O,--oldid", old_id, "Use an old version");
  app.add_flag("-r,--render", render, "Output parser rendered html");
  app.add_flag("-s,--silent", silent, "Silent mode");
  app.add_option("-l,--login", login, "Username for editing");
  app.add_option("-p,--password", password, "Password for editing");
  app.validate_positionals();
  app.validate_optional_arguments();

  CLI11_PARSE(app, argc, argv);

  if (silent)
    cerr.setstate(std::ios_base::failbit);

  cerr << "Fetching " << page_name << "..." << flush;

  MWAPI::API api;
  string bytes = api.get_page_content(page_name);
  cerr << " OK (Received " << bytes.size() << " bytes)" << endl;
  cerr << "Parsing..." << flush;
  Wiki::Wikitext wikitext(bytes);
  Wiki::Blocks blocks = wikitext.decode();
  cerr << " OK (" << blocks.size() << " blocks)" << endl;

  if (!no_fix)
  {
    cerr << "Fixing..." << endl;
    int fix_count = 0;
    for (int i = 0; i < 3; i++)
    {
      Fixes::space_unnecessary(blocks, fix_count);
      Fixes::footnote_position(blocks, fix_count);
      Fixes::punc_duplicate(blocks, fix_count);
      Fixes::punc_wrong_width(blocks, fix_count);
    }

    if (!fix_count)
    {
      cerr << "Not needed" << endl;
    }
    else if (fix_count == 1)
    {
      cerr << fix_count << " fix applied." << endl;
    }
    else if (fix_count > 1)
    {
      cerr << fix_count << " fixes applied." << endl;
    }

    wikitext = Wiki::Wikitext(blocks);

    if (edit && fix_count > 0)
    {
      cerr << "Authenticating..." << flush;
      string name = api.login(login, password);
      cerr << " OK (Logged in as " << name << ")" << endl;
      cerr << "Pushing changes..." << flush;
      string revid = api.edit_page(page_name, wikitext.to_string(), "[[User:SkEy/wikitext-fix|wikitext-fix]]: 维基文本自动修复工具", true);
      cerr << " OK (Revision id: " << revid << ")" << endl;
    }
  }

  if (render)
    cout << wikitext.color_html() << endl;

  return EXIT_SUCCESS;
}

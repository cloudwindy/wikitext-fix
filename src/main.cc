#include <iostream>
#include <fstream>

#include <CLI/CLI.hpp>

#include "api.hh"
#include "wikitext.hh"
#include "fix.hh"
#include "utf-32.hh"

using std::cout, std::cerr, std::endl, std::flush;
using std::string;
using ustring = std::u32string;

int main(int argc, char *argv[])
{
  CLI::App app;

  string page_name, output_path, login, password, wiki;
  int old_id = 0;
  bool edit = false, render = false, no_fix = false, silent = false;
  app.add_option("pos", page_name, "Page title name")->required();
  app.add_flag("-e,--edit", edit, "Edit page and suppress output");
  app.add_flag("-n,--no-fix", no_fix, "Disable fixing");
  app.add_option("-o,--output", output_path, "Write to file instead of stdout");
  app.add_option("-O,--oldid", old_id, "Use an old version");
  app.add_flag("-r,--render", render, "Output parser rendered html");
  app.add_flag("-s,--silent", silent, "Silent mode");
  app.add_option("-l,--login", login, "Username for editing");
  app.add_option("-p,--password", password, "Password for editing");
  app.add_option("--wiki", wiki, "Use custom wiki instead of Chinese Wikipedia (Use with caution!)")->default_str("zh.wikipedia.org");
  app.validate_positionals();
  app.validate_optional_arguments();

  CLI11_PARSE(app, argc, argv);

  if (silent)
    cerr.setstate(std::ios_base::failbit);

  cerr << "Fetching " << page_name << "..." << flush;

  string base_url("https://" + wiki + "/w/api.php");
  MWAPI::API api(base_url);
  string bytes = api.get_page_content(page_name);
  cerr << " OK (Received " << bytes.size() << " bytes)" << endl;
  cerr << "Parsing..." << flush;
  Wiki::Wikitext wikitext(bytes);
  Wiki::Blocks blocks = wikitext.decode();
  cerr << " OK (" << blocks.size() << " blocks)" << endl;

  if (!no_fix)
  {
    cerr << "Fixing..." << endl;
    Fix fix(blocks);
    for (int i = 0; i < 3; i++)
    {
      fix.space_unnecessary();
      fix.footnote_position();
      fix.punc_duplicate();
      fix.punc_wrong_width();
    }

    if (!fix.count)
    {
      cerr << "Not needed" << endl;
    }
    else if (fix.count == 1)
    {
      cerr << fix.count << " fix applied." << endl;
    }
    else if (fix.count > 1)
    {
      cerr << fix.count << " fixes applied." << endl;
    }

    wikitext = Wiki::Wikitext(blocks);

    if (edit && fix.count > 0)
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
  else if (!edit)
    cout << wikitext << endl;

  return EXIT_SUCCESS;
}

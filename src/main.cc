#include <iostream>

#include <curl/curl.h>
#include <boost/program_options.hpp>

#include "wikipedia.hh"
#include "wikitext.hh"
#include "fixes.hh"
#include "utf-32.hh"

using std::cout, std::cerr, std::endl;
using std::string;
using ustring = std::u32string;
namespace po = boost::program_options;

int main(int ac, char *av[])
{
  curl_global_init(CURL_GLOBAL_ALL);
  curl_version_info_data *curl_ver = curl_version_info(CURLVERSION_NOW);
  cerr << "libcurl version " << curl_ver->version << endl;

  po::options_description desc_visible("Options");
  auto opt = desc_visible.add_options();
  opt("help", "show help message");
  opt("render", "output parser rendered html instead of wikitext");
  opt("fix-notes", "fix footnotes location");
  opt("fix-punc", "remove duplicate punctuation");
  opt("fix-punc-width", "correct wrong punctuation width");
  opt("fix-space", "remove spaces between Chinese and English");

  po::options_description desc_hidden("Hidden Options");
  desc_hidden.add_options()("page-name", "page name");

  po::options_description desc;
  desc.add(desc_visible).add(desc_hidden);

  po::positional_options_description pos_desc;
  pos_desc.add("page-name", -1);

  po::variables_map vm;
  try
  {
    // Only parse the options, so we can catch the explicit `--page-name`
    auto parsed = po::command_line_parser(ac, av)
                      .options(desc)
                      .positional(pos_desc)
                      .run();

    // Make sure there were no non-positional `page-name` options
    for (auto const &opt : parsed.options)
    {
      if ((opt.position_key == -1) && (opt.string_key == "page-name"))
      {
        throw po::unknown_option("page-name");
      }
    }

    po::store(parsed, vm);
    po::notify(vm);
  }
  catch (const po::error &e)
  {
    cerr << "Couldn't parse command line arguments properly:\n";
    cerr << e.what() << '\n'
         << '\n';
    cerr << "Usage: " << av[0] << " [options] <page-name> [options]\n";
    cerr << desc_visible << endl;
    return 1;
  }

  if (vm.count("help"))
  {
    cerr << "Usage: " << av[0] << " [options] <page-name> [options]\n";
    cout << desc_visible << "\n";
    return 1;
  }

  string page_name;
  if (vm.count("page-name"))
  {
    page_name = vm["page-name"].as<string>();
  }

  if (page_name.empty())
  {
    cerr << "Run " << av[0] << " --help for help." << endl;
    return 1;
  }
  cerr << "Fetching " << page_name << "..." << endl;
  string bytes = Wikipedia::page_wikitext(page_name);
  if (bytes.starts_with("#REDIRECT"))
  {
    page_name = bytes.substr(bytes.find("[[") + 2);
    page_name = page_name.substr(0, page_name.rfind("]]"));
    cerr << "Redirecting to " << page_name << "..." << endl;
    bytes = Wikipedia::page_wikitext(page_name);
  }
  cerr << "Parsing..." << endl;
  Wiki::Wikitext wikitext(bytes);
  Wiki::UBlocks ublocks = wikitext.decode();
  cerr << "Fixing... (" << wikitext.size() << " blocks)" << endl;

  int fix_count = 0;

  if (vm.count("fix-notes"))
    Fixes::footnotes(ublocks, fix_count);

  if (vm.count("fix-punc"))
    Fixes::punctuation(ublocks, fix_count);

  if (vm.count("fix-punc-width"))
    Fixes::punctuation_width(ublocks, fix_count);

  if (vm.count("fix-space"))
    Fixes::space(ublocks, fix_count);

  if (!fix_count)
    cerr << "No need to fix." << endl;
  else if (fix_count == 1)
    cerr << "1 fix applied." << endl;
  else if (fix_count > 1)
    cerr << fix_count << " fixes applied." << endl;

  wikitext = Wiki::Wikitext(ublocks);

  if (vm.count("render"))
    cout << wikitext.color_html() << endl;
  else
    cout << wikitext << endl;

  curl_global_cleanup();
}

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

  po::options_description desc("Options");
  auto opt = desc.add_options();
  opt("help", "show help message");
  opt("page-name", "page name (defaults to first positional arg)");
  opt("fix-note", "fix footnotes location");
  opt("fix-punc", "fix punctuation");
  opt("fix-space", "fix spaces between Chinese and English");

  po::positional_options_description pos_desc;
  pos_desc.add("page-name", -1);

  po::variables_map vm;
  auto parsed = po::command_line_parser(ac, av)
                    .options(desc)
                    .positional(pos_desc)
                    .run();
  po::store(parsed, vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    cout << desc << "\n";
    return 1;
  }

  string page_name;
  if (vm.count("page-name"))
  {
    page_name = vm["page-name"].as<string>();
  }
  if (page_name.empty())
  {
    cerr << "page name not specified" << endl;
    return 1;
  }
  string bytes = Wikipedia::page_wikitext(page_name);
  ustring str = converter.from_bytes(bytes);

  if (vm.count("fix-note"))
    Fixes::footnotes(str);

  if (vm.count("fix-punc"))
    Fixes::punctuation(str);

  if (vm.count("fix-space"))
    Fixes::space(str);

  bytes = converter.to_bytes(str);
  WikitextParser::print(bytes);
  // cout << bytes << endl;

  curl_global_cleanup();
}

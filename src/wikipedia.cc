#include "wikipedia.hh"
#include <vector>
#include <stdexcept>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

using std::string, std::vector;
using error = std::runtime_error;

static string http_get(string url);

namespace Wikipedia
{
  static string page_wikitext_url(const string page_name);

  string page_wikitext(string page_name)
  {
    const string url = page_wikitext_url(page_name);
    const string resp = http_get(url);
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(resp, root))
    {
      throw error("invalid json");
    }

    return root["parse"]["wikitext"].asString();
  }

  static string page_wikitext_url(const string page_name)
  {
    CURLU *u = curl_url();
    if (!u)
    {
      throw error("curl_url returned null");
    }
    curl_url_set(u, CURLUPART_URL, WIKIPEDIA_API_URL, 0);
    vector<string> queries{
        "action=parse",
        "page=" + page_name,
        "prop=wikitext",
        "maxlag=5",
        "format=json",
        "formatversion=2"};
    for (const auto &query : queries)
    {
      curl_url_set(u, CURLUPART_QUERY, query.c_str(), CURLU_APPENDQUERY | CURLU_URLENCODE);
    }
    char *url_ptr;
    curl_url_get(u, CURLUPART_URL, &url_ptr, 0);
    curl_url_cleanup(u);

    string url(url_ptr);
    curl_free(url_ptr);

    return url;
  }
} // namespace Wikipedia

static size_t curl_cpp_write_callback(const char *data, size_t size, size_t nitems, void *out_ptr);

static string http_get(string url)
{
  CURL *c = curl_easy_init();
  if (!c)
  {
    throw error("curl_easy_init returned null");
  }
  curl_slist *headers = curl_slist_append(nullptr, "");
  curl_easy_setopt(c, CURLOPT_URL, url.c_str());
  curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 3L);
#ifdef ENABLE_CURL_VERBOSE
  curl_easy_setopt(c, CURLOPT_VERBOSE, 1L);
#endif
  curl_easy_setopt(c, CURLOPT_ACCEPT_ENCODING, ""); // Allow any supported encoding.
  curl_easy_setopt(c, CURLOPT_USERAGENT, "wikitext-fix-bot/1.0");
  curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

  // C++ specific
  string resp;
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, curl_cpp_write_callback);
  curl_easy_setopt(c, CURLOPT_WRITEDATA, &resp);

  const CURLcode code = curl_easy_perform(c);
  curl_easy_cleanup(c);
  curl_slist_free_all(headers);
  if (code != CURLE_OK)
  {
    throw error(string("curl_easy_perform returned ") + std::to_string(code));
  }

  return resp;
}

static size_t curl_cpp_write_callback(const char *data, size_t size, size_t nitems, void *out_ptr)
{
  auto str = static_cast<string *>(out_ptr);
  str->append(data, size * nitems);
  return size * nitems;
}
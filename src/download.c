#include "download.h"

#include <assert.h>
#include <libgen.h> // For basename()
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>


//
// Forward declarations
//

size_t fetchdeps_download_writefunc(void* buffer, size_t size, size_t nmemb, void* userdata);

bool_t fetchdeps_download_fetch_one(CURL* curl, char* url, char* to_dir);

char* fetchdeps_download_get_local_filename(char* url, char* to_dir);


//
// Public functions
//

bool_t
fetchdeps_download_fetch_all(stringset_t* urls, char* to_dir)
{
  CURL* curl = NULL;
  stringiter_t* url_iter = NULL;
  char* url;

  assert(urls);
  assert(to_dir);

  // TODO: Check that the to_dir exists and is writable.

  curl = curl_easy_init();
  if (!curl)
    goto failure;

  url_iter = fetchdeps_stringiter_new(urls);
  if (!url_iter)
    goto failure;

  // Set up common curl options.
  if (curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetchdeps_download_writefunc) != CURLE_OK)
    goto failure;
  if (curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0) != CURLE_OK)
    goto failure;

  // Loop over the URLs, downloading each in turn.
  url = fetchdeps_stringiter_next(url_iter);
  while (url) {
    if (!fetchdeps_download_fetch_one(curl, url, to_dir))
      goto failure;
    url = fetchdeps_stringiter_next(url_iter);
  }

  fetchdeps_stringiter_free(url_iter);
  curl_easy_cleanup(curl);

  return 1;

failure:
  if (url_iter)
    fetchdeps_stringiter_free(url_iter);
  if (curl)
    curl_easy_cleanup(curl);
  return 0;
}


//
// Internal functions
//

size_t
fetchdeps_download_writefunc(void *buffer, size_t size, size_t nmemb, void *userp)
{
  FILE* f = (FILE*)userp;
  assert(f != NULL);
  return fwrite(buffer, size, nmemb, f);
}


bool_t
fetchdeps_download_fetch_one(CURL* curl, char* url, char* to_dir)
{
  char* local_filename = NULL;
  FILE* local_file = NULL;

  // Figure out what to save the file as locally.
  local_filename = fetchdeps_download_get_local_filename(url, to_dir);
  if (!local_filename)
    goto failure;

  local_file = fopen(local_filename, "wb");
  if (!local_file)
    goto failure;

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, local_file);

  if (curl_easy_perform(curl) != CURLE_OK)
    goto failure;

  fclose(local_file);
  free(local_filename);

  return 1;

failure:
  if (local_file)
    fclose(local_file);
  if (local_filename)
    free(local_filename);
  return 0;
}


char*
fetchdeps_download_get_local_filename(char* url, char* to_dir)
{
  char* url_copy = NULL;
  char* filename = NULL;
  char* local_path = NULL;
  int local_path_len;

  url_copy = strdup(url);
  if (!url_copy)
    goto failure;

  filename = basename(url_copy);
  if (!filename)
    goto failure;
  free(url_copy);
  url_copy = NULL;

  local_path_len = strlen(to_dir) + strlen(filename) + 2;
  local_path = malloc(local_path_len * sizeof(char));
  if (!local_path)
    goto failure;

  if (snprintf(local_path, local_path_len, "%s/%s", to_dir, filename) != local_path_len - 1)
    goto failure;

  return local_path;

failure:
  if (url_copy)
    free(url_copy);
  if (local_path)
    free(local_path);
  return NULL;
}


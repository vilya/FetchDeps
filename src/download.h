#ifndef fetchdeps_download_h
#define fetchdeps_download_h

#include "stringset.h"

//
// Public functions
//

// Download the contents of a set of URLs. If any of the downloads fails for
// any reason, the return value will be false; otherwise it will be true.
//
// The to_dir parameter is the path to a directory where all the downloaded
// files will be stored. If the directory doesn't exist, or doesn't have both
// read and write permission for the current user, the function will return
// false without trying to download anything.
bool_t fetchdeps_download_fetch_all(stringset_t* urls, char* to_dir);

#endif // fetchdeps_download_h


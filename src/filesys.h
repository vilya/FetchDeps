#ifndef fetchdeps_filesys_h
#define fetchdeps_filesys_h

#include "common.h"

//
// Functions
//

// Check whether the given path is a directory. Returns true if it is; returns
// false if the path doesn't exist, it isn't a directory, the current user has
// insufficient permission to open the directory, or there was any other
// failure.
bool_t fetchdeps_filesys_is_directory(char* path);

// Find the default deps file by searching upwards through the directory
// hierarchy, starting from the current directory. The default file name is
// "default.deps". If we find a readable file with that name in our search, we
// return the full (canonical) path as a null-terminated string. This string
// will have been allocated using malloc & the caller must free it themselves.
// If the file was not found, the return value is NULL.
char* fetchdeps_filesys_default_deps_file();

// Returns the default download directory. On Linux and OS X this is a
// directory called ".deps" in the same directory as the deps_file. On Windows
// it's currently the same but is likely to change to something more
// appropriate for that platform in the future. You should always use this
// function to get the directory name rather than relying on it having a
// particular value.
//
// The deps_file parameter may be an absolute or relative path, but must not be
// NULL. The return value will be NULL if the deps_file doesn't exist or some
// other error occurred.
char* fetchdeps_filesys_default_download_dir(char* deps_file);

// Create a directory with the given name. This assumes all the parent
// directories already exist; the function will fail if they don't, rather than
// attempting to create them.
//
// The return value indicates whether the function succeeded or failed. If it's
// true, then the directory was created successfully. If it's false, the
// directory couldn't be created because it's parent directory didn't exist, the
// current user didn't have permission to create a directory in that location, a
// directory with the same name for which the current user doesn't have read and
// write permission already exists, or the path names a file.
//
// If the path is the name of an existing directory for which the user has read
// and write permission, this function returns true without doing anything
// further.
bool_t fetchdeps_filesys_make_directory(char* path);

#endif // fetchdeps_filesys_h


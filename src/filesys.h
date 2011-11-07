#ifndef fetchdeps_filesys_h
#define fetchdeps_filesys_h

//
// Functions
//

// Find the default deps file by searching upwards through the directory
// hierarchy, starting from the current directory. The default file name is
// "default.deps". If we find a readable file with that name in our search, we
// return the full (canonical) path as a null-terminated string. This string
// will have been allocated using malloc & the caller must free it themselves.
// If the file was not found, the return value is NULL.
char* fetchdeps_filesys_default_deps_file();

// Returns the default download directory: a directory called 'Thirdparty' in
// the same directory as the deps_file. The deps_file parameter may be an
// absolute or relative path, but must not be NULL.
//
// The return value will be NULL if the deps_file doesn't exist or some other
// error occurred.
char* fetchdeps_filesys_default_download_dir(char* deps_file);


#endif // fetchdeps_filesys_h


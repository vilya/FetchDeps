#ifndef fetchdeps_common_h
#define fetchdeps_common_h


//
// Constants
//

#if defined(_WIN64) || defined(_WIN32) || defined(__CYGWIN__)
  #define kOperatingSystem  "win"
#elif defined(__linux__)
  #define kOperatingSystem  "linux"
#elif defined(__APPLE__)
  #define kOperatingSystem  "mac"
#endif


//
// Typedefs
//

typedef int bool_t;


#endif // fetchdeps_common_h


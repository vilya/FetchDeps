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

typedef int Bool;
static const Bool False = 0;
static const Bool True = ~0;


//
// Functions
//

int CopyStr(char* to, const char* from, int numChars);

#endif // fetchdeps_common_h


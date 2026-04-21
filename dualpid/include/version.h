#ifndef __VERSION_H
#define __VERSION_H

// _version is generated during build:
#include "_version.h"

#define VERSION_MAJOR           2
#define VERSION_MINOR           5

#define _STRINGIFY(x)           #x
#define STRINGIFY(x)            _STRINGIFY(x)
#define FLOATIFY(integ, fract)  (integ##.##fract##f)
#define VERSION_MAJOR_STR       STRINGIFY(VERSION_MAJOR)
#define VERSION_MINOR_STR       STRINGIFY(VERSION_MINOR)
#define VERSION_NUMBER          ((VERSION_MAJOR << 16) | VERSION_MINOR)
#define VERSION_FLOAT           FLOATIFY(VERSION_MAJOR, VERSION_MINOR)

#if GIT_BRANCH_MAIN
    #define VERSION_PATCH         BUILD_DATE_YM_STR
    // #if GIT_DIRTY
    //     #pragma message "Compiling main branch release from dirty git checkout!"
    // #endif
    #define VERSION_STR           VERSION_MAJOR_STR "." VERSION_MINOR_STR "." VERSION_DIRTY_STR
#else
    #define VERSION_PATCH         GIT_REVISION
    #define VERSION_STR           GIT_BRANCH "-" VERSION_PATCH VERSION_DIRTY_STR
#endif

#endif // __VERSION_H

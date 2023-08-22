#ifndef __VERSION_H
#define __VERSION_H

#define _STRINGIFY(x)      #x
#define STRINGIFY(x)      _STRINGIFY(x)

#define VERSIONSTR_LONG         STRINGIFY(GIT_BRANCH-VERSION)
#define VERSIONSTR_SHORT        STRINGIFY(VERSION)
// #define BUILD_DATESTR_YMD       STRINGIFY(BUILD_DATE_YMD)
// #define BUILD_DATESTR_YM        STRINGIFY(BUILD_DATE_YM)
#define VERSION                 BUILD_DATE_YMD
#if GET_BRANCH == master
    #define VERSION_STR        "dev" STRINGIFY(BUILD_DATE_YMD)
#else
    #define VERSION_STR             STRINGIFY(GIT_BRANCH) " (" STRINGIFY(BUILD_DATE_YM) ")"
#endif

#endif // __VERSION_H

#ifndef __VERSION_H
#define __VERSION_H

// A few defines here are set during build in def_version.py

#define _STRINGIFY(x)      #x
#define STRINGIFY(x)      _STRINGIFY(x)

#define VERSIONSTR_LONG         STRINGIFY(GIT_BRANCH-VERSION)
#define VERSIONSTR_SHORT        STRINGIFY(VERSION)
// #define BUILD_DATESTR_YMD       STRINGIFY(BUILD_DATE_YMD)
// #define BUILD_DATESTR_YM        STRINGIFY(BUILD_DATE_YM)
#define VERSION                 BUILD_DATE_YMD
#define VERSION_STR             STRINGIFY(GIT_BRANCH) " (" STRINGIFY(BUILD_DATE_YM) ")"

#endif // __VERSION_H

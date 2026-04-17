import datetime
import os
import re
import subprocess
from sys import stderr

# Version 17-4-2026

INPUT_PATH = "include/version.h"
OUTPUT_PATH = "include/_version.h"
DIRTY_STRING = "-dirty"

# Read VERSION_MAJOR / VERSION_MINOR from version.h
with open(INPUT_PATH) as f:
    src = f.read()
VERSION_MAJOR = int(re.search(r'#define\s+VERSION_MAJOR\s+(\d+)', src).group(1))
VERSION_MINOR = int(re.search(r'#define\s+VERSION_MINOR\s+(\d+)', src).group(1))

curr_date = datetime.datetime.now()
build_date_ym  = "%02d%02d"     % (curr_date.year % 100, curr_date.month)
build_date_ymd = "%02d%02d%02d" % (curr_date.year % 100, curr_date.month, curr_date.day)

try:
    git_revision = (
        subprocess.check_output(["git", "rev-parse", "HEAD"])
        .strip()
        .decode("utf-8")
    )[-8:]
except subprocess.CalledProcessError:
    print("Warning: Could not get git commit", file=stderr)
    git_revision = "unknown"

try:
    git_branch = (
        subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
        .strip()
        .decode("utf-8")
    )
except subprocess.CalledProcessError:
    print("Warning: Could not get git branch", file=stderr)
    git_branch = "unknown"

try:
    dirty = bool(subprocess.check_output(["git", "status", "--porcelain"]).strip())
except subprocess.CalledProcessError:
    dirty = False

dirty_suffix  = DIRTY_STRING if dirty else ""
branch_main   = git_branch in ("main", "master")

if branch_main:
    version_str = "%d.%d.%s%s" % (VERSION_MAJOR, VERSION_MINOR, build_date_ym, dirty_suffix)
else:
    version_str = "%s-%s%s" % (git_branch, build_date_ymd, dirty_suffix)

content = (
    "#ifndef __VERSION_AUTO_H\n"
    "#define __VERSION_AUTO_H\n\n"
    "#define BUILD_DATE_YM      %s\n"
    "#define BUILD_DATE_YM_STR  \"%s\"\n"
    "#define BUILD_DATE_YMD     %s\n"
    "#define BUILD_DATE_YMD_STR \"%s\"\n"
    "#define GIT_REVISION       \"%s\"\n"
    "#define GIT_BRANCH         \"%s\"\n"
    "#define GIT_DIRTY          %d\n"
    "#define GIT_BRANCH_MAIN    %d\n"
    "#define VERSION_DIRTY_STR  \"%s\"\n"
    "#define VERSION_STR_AUTO   \"%s\"\n"
    "\n#endif // __VERSION_AUTO_H\n"
) % (build_date_ym, build_date_ym, build_date_ymd, build_date_ymd,
     git_revision, git_branch, dirty, branch_main, dirty_suffix, version_str)

# Only write the file if the contents actually changed:
if not os.path.exists(OUTPUT_PATH) or open(OUTPUT_PATH).read() != content:
    with open(OUTPUT_PATH, "w") as f:
        f.write(content)
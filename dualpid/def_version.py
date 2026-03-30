import datetime
import os
import subprocess
from sys import stderr

curr_date = datetime.datetime.now()
build_datetime = "%02d%02d%02d" % (curr_date.year, curr_date.month, curr_date.day)
build_date_ymd = curr_date.year * 10000 + curr_date.month * 100 + curr_date.day
build_date_ym  = curr_date.year * 100   + curr_date.month

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

content = (
    "#ifndef __VERSION_AUTO_H\n"
    "#define __VERSION_AUTO_H\n\n"
    "#define BUILD_DATE_YMD %d\n"
    "#define BUILD_DATE_YMD_STR \"%s\"\n"
    "#define BUILD_DATE_YM %d\n"
    "#define BUILD_DATE_YM_STR \"%s\"\n"
    "#define GIT_REVISION \"%s\"\n"
    "#define GIT_BRANCH \"%s\"\n"
    "\n#endif //__VERSION_AUTO_H"
) % (build_date_ymd, build_date_ymd, build_date_ym, build_date_ym, git_revision, git_branch)

# Only write the file if the contents actually changed:
path = "include/version_auto.h"
if not os.path.exists(path) or open(path).read() != content:
    with open(path, "w") as f:
        f.write(content)

import datetime
import os
import subprocess
from sys import stderr

output_path = "include/version_auto.h"

curr_date = datetime.datetime.now()
build_date_ym = "%02d%02d" % (curr_date.year, curr_date.month)
build_date_ymd = "%02d%02d%02d" % (curr_date.year, curr_date.month, curr_date.day)

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
    "#define BUILD_DATE_YM %s\n"
    "#define BUILD_DATE_YM_STR \"%s\"\n"
    "#define BUILD_DATE_YMD %s\n"
    "#define BUILD_DATE_YMD_STR \"%s\"\n"
    "#define GIT_REVISION \"%s\"\n"
    "#define GIT_BRANCH \"%s\"\n"
    "\n#endif // __VERSION_AUTO_H\n"
) % (build_date_ym, build_date_ym, build_date_ymd, build_date_ymd, 
     git_revision, git_branch)

# Only write the file if the contents actually changed:
if not os.path.exists(output_path) or open(output_path).read() != content:
    with open(output_path, "w") as f:
        f.write(content)

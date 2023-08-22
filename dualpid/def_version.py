import subprocess
import datetime

#(part of) GIT revision
revision = (
    subprocess.check_output(["git", "rev-parse", "HEAD"])
    .strip()
    .decode("utf-8")
)

branch = (
    subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    .strip()
    .decode("utf-8")
)

#date
curr_date = datetime.datetime.now()
date_ymd = "%02d%02d%02d" % (curr_date.year % 100, curr_date.month, curr_date.day)
date_ym = "%02d%02d" % (curr_date.year % 100, curr_date.month)
devbuild = ""
if(branch in ['master', 'main']):
    devbuild = "-DDEVELOPMENT_BUILD"

print("-DBUILD_DATE_YMD=%s -DBUILD_DATE_YM=%s -DGIT_BRANCH=%s -DGIT_REVISION=%s %s" % (date_ymd, date_ym, branch, revision[-8:], devbuild))

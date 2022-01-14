import subprocess
import datetime

#(part of) GIT revision
# version = (
#     subprocess.check_output(["git", "rev-parse", "HEAD"])
#     .strip()
#     .decode("utf-8")
# )
# print("-DVERSION='\"%s\"'" % version[-4:])

#date
curr_date = datetime.datetime.now()
version = "%02d%02d%02d" % (curr_date.year, curr_date.month, curr_date.day)
print("-DVERSION=%s" % version)

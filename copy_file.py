from shutil import copyfile
import sys

src_file = ""
dst_file = ""
if len(sys.argv) == 3:
    src_file = sys.argv[1]
    dst_file = sys.argv[2]
else:
    raise ParamsParseError("Wrong amount of command line options")

copyfile(src_file, dst_file)
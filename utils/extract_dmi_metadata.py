import sys
import subprocess
import json
import StringIO

if len(sys.argv) < 2:
    print("Dmi file is missing!")
    exit(-1)

print("Extracting metadata from '" + sys.argv[1] + "'...")
output = subprocess.check_output('identify -format "%[Description]" "' + sys.argv[1] + '"', shell=True)
metadata_io = StringIO.StringIO(output)
for line in metadata_io:
    # TODO: to json
    print(line)

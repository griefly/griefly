import sys
import subprocess
import json
import StringIO

if len(sys.argv) < 2:
    print("Dmi file is missing!")
    exit(-1)

metadata = {"info" : {},
            "states" : {}}

def extract_key_value(line):
    index = line.index('=')
    key = line[:index].strip(' \t\n\r')
    value = line[index + 1:].strip(' \t\n\r')  
    return (key, value) 

print("Extracting metadata from '" + sys.argv[1] + "'...")
output = subprocess.check_output('identify -format "%[Description]" "' + sys.argv[1] + '"', shell=True)
if "BEGIN DMI" not in output or "END DMI" not in output:
    raise ParseError("Dmi metadata is corrupted!")

metadata_io = StringIO.StringIO(output)
current_object = metadata["info"] 
for line in metadata_io:
    if "BEGIN DMI" in line:
        continue
    if "END DMI" in line:
        continue
    (key, value) = extract_key_value(line)
    if key == "state":
        if not value.startswith('"') or not value.endswith('"'):
            raise ParseError("State '" + value + "' does not have quotes around itself")
        value = value[1:-1]
        metadata["states"][value] = {}
        current_object = metadata["states"][value]
        continue;
    current_object[key] = value

json_data = json.JSONEncoder(indent = 4).encode(metadata)
json_name = sys.argv[1] + ".json"
with open(json_name, "w") as json_file:
    json_file.write(json_data)
print("Metadata was successfully written to '" + json_name + "'")

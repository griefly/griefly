from __future__ import print_function

import os
import sys
import json

with open("metadata.json", "r") as json_file:
    raw_json = json_file.read()

metadata = json.JSONDecoder().decode(raw_json)

header_list = []

for class_data in metadata["classes"]:
    header = class_data["header"]
    if header not in header_list:
        header_list.append(header)

with open("Sources/AutogenMetadata.cpp", "w") as autogen_file:
    for header in header_list:
        print('#include "' + header + '"', file = autogen_file)
    print("", file = autogen_file)
    print("void InitRealTypes()", file = autogen_file)
    print("{", file = autogen_file)
    for class_data in metadata["classes"]:
        index = str(metadata["classes"].index(class_data))
        print("    " + class_data["class"] + "::REAL_TYPE_ITEM = " + index + ";", file = autogen_file)
    print("}", file = autogen_file)

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
    print('#include "AutogenMetadata.h"', file = autogen_file)
    print('#include "Idptr.h"', file = autogen_file)
    print("", file = autogen_file)
    for header in header_list:
        print('#include "' + header + '"', file = autogen_file)
    print("", file = autogen_file)
    print("std::unordered_map<std::string, setters_for_type>& get_setters_for_types()", file = autogen_file)
    print("{", file = autogen_file)
    print("    static std::unordered_map<std::string, setters_for_type> map;", file = autogen_file)
    print("    return map;", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    print("int IMainObject::REAL_TYPE_ITEM;", file = autogen_file)
    for class_data in metadata["classes"]:
        print("int " + class_data["class"] + "::REAL_TYPE_ITEM;", file = autogen_file)
    print("", file = autogen_file)
    print("void InitRealTypes()", file = autogen_file)
    print("{", file = autogen_file)
    print("    IMainObject::REAL_TYPE_ITEM = 0;", file = autogen_file)
    for class_data in metadata["classes"]:
        index = str(metadata["classes"].index(class_data) + 1)
        print("    " + class_data["class"] + "::REAL_TYPE_ITEM = " + index + ";", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    print("void InitCreators()", file = autogen_file)
    print("{", file = autogen_file)
    for class_data in metadata["classes"]:
        class_name = class_data["class"]
        print("    (*itemList())[" + class_name + "::T_ITEM_S()] = &" + class_name + "::_Z_creator;", file = autogen_file)
        print("    (*itemListSaved())[" + class_name + "::T_ITEM_S()] = &" + class_name + "::_Z_creatorSaved;", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    print("void InitSettersForTypes()", file = autogen_file)
    print("{", file = autogen_file)
    for class_data in metadata["classes"]:
        for variable in class_data["variables"]:
            print('    get_setters_for_types()["' + class_data["class"] + '"]["' + variable + '"] = &' + class_data["class"] + '::_Z_KV_SETTERS' + variable + ';', file = autogen_file)
        #print("", file = autogen_file)
    print("}", file = autogen_file)
    

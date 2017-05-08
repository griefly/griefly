from __future__ import print_function

import os
import sys
import json

print('Autogen started')

with open("metadata.json", "r") as json_file:
    raw_json = json_file.read()

metadata = json.JSONDecoder().decode(raw_json)

def get_class_data(class_name):
    for class_data in metadata["classes"]:
        if class_data["class"] == class_name:
            return class_data
    return None

header_list = []

for class_data in metadata["classes"]:
    header = class_data["header"]
    if header not in header_list:
        header_list.append(header)

with open("AutogenMetadata.cpp", "w") as autogen_file:
    print('#include "AutogenMetadata.h"', file = autogen_file)
    print('#include "core/Idptr.h"', file = autogen_file)
    print("", file = autogen_file)
    for header in header_list:
        print('#include "' + header + '"', file = autogen_file)
    print("", file = autogen_file)
    print("using namespace kv;", file = autogen_file)
    print("", file = autogen_file)
    print("std::unordered_map<QString, SettersForType>& GetSettersForTypes()", file = autogen_file)
    print("{", file = autogen_file)
    print("    static std::unordered_map<QString, SettersForType> map;", file = autogen_file)
    print("    return map;", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    print("int Object::TYPE_INDEX;", file = autogen_file)
    print("const QString& Object::GetTypeStatic()", file = autogen_file)
    print("{", file = autogen_file)
    print('    static const QString type = "main";', file = autogen_file)
    print("    return type;", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    for class_data in metadata["classes"]:
        print("int " + class_data["class"] + "::TYPE_INDEX;", file = autogen_file)
        print("const QString& " + class_data["class"] + "::GetTypeStatic()", file = autogen_file)
        print("{", file = autogen_file)
        print('    static const QString type = "' + class_data["type"] + '";', file = autogen_file)
        print("    return type;", file = autogen_file)
        print("}", file = autogen_file)
        print("", file = autogen_file)
    print("void InitRealTypes()", file = autogen_file)
    print("{", file = autogen_file)
    print("    Object::TYPE_INDEX = 0;", file = autogen_file)
    for class_data in metadata["classes"]:
        index = str(metadata["classes"].index(class_data) + 1)
        print("    " + class_data["class"] + "::TYPE_INDEX = " + index + ";", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    print("void InitCreators()", file = autogen_file)
    print("{", file = autogen_file)
    print("    (*GetItemsCreators())[Object::GetTypeStatic()] = &Object::_Z_creator;", file = autogen_file)
    print("    (*GetVoidItemsCreators())[Object::GetTypeStatic()] = &Object::_Z_creatorSaved;", file = autogen_file)
    for class_data in metadata["classes"]:
        class_name = class_data["class"]
        print("    (*GetItemsCreators())[" + class_name + "::GetTypeStatic()] = &" + class_name + "::_Z_creator;", file = autogen_file)
        print("    (*GetVoidItemsCreators())[" + class_name + "::GetTypeStatic()] = &" + class_name + "::_Z_creatorSaved;", file = autogen_file)
    print("}", file = autogen_file)
    print("", file = autogen_file)
    print("void InitSettersForTypes()", file = autogen_file)
    print("{", file = autogen_file)
    for class_data in metadata["classes"]:
        class_data_loc = class_data
        while class_data_loc:
            for variable in class_data_loc["variables"]:
                print("    GetSettersForTypes()[" + class_data["class"] + '::GetTypeStatic()]["' + variable + '"] = &' + class_data_loc["class"] + '::_Z_KV_SETTERS' + variable + ';', file = autogen_file)
            class_data_loc = get_class_data(class_data_loc["base_class"])
        #print("", file = autogen_file)
    print("}", file = autogen_file)
print('AutogenMetadata.cpp file has been generated')

with open("AutogenSerialization.cpp", "w") as autogen_file:
    for header in header_list:
        print('#include "' + header + '"', file = autogen_file)
    print("", file = autogen_file)
    print("using namespace kv;", file = autogen_file)
    print("", file = autogen_file)
    for class_data in metadata["classes"]:
        print("", file = autogen_file)
        print("void " + class_data["class"] + "::Save(FastSerializer& file) ", file = autogen_file)
        print("{", file = autogen_file)
        print("    " + class_data["base_class"] + "::Save(file);", file = autogen_file)
        if len(class_data["variables"]):
            print("", file = autogen_file)
        for variable in class_data["variables"]:
            print("    WrapWriteMessage(file, " + variable + ");", file = autogen_file)
        print("}", file = autogen_file)
        print("", file = autogen_file)
        print("void " + class_data["class"] + "::Load(FastDeserializer& file) ", file = autogen_file)
        print("{", file = autogen_file)
        print("    " + class_data["base_class"] + "::Load(file);", file = autogen_file)
        if len(class_data["variables"]):
            print("", file = autogen_file)
        for variable in class_data["variables"]:
            print("    WrapReadMessage(file, " + variable + ");", file = autogen_file)
        if len(class_data["on_load_calls"]):
            print("", file = autogen_file)
        for function in class_data["on_load_calls"]:
            print("    " + function + "();", file = autogen_file)
        print("}", file = autogen_file)
        print("", file = autogen_file)
        print("unsigned int " + class_data["class"] + "::Hash() ", file = autogen_file)
        print("{", file = autogen_file)
        print("    unsigned int retval = 0;", file = autogen_file)
        print("    retval += " + class_data["base_class"] + "::Hash();", file = autogen_file)
        for variable in class_data["variables"]:
            print("    retval += hash(" + variable + ");", file = autogen_file)
        print("    return retval;", file = autogen_file)
        print("}", file = autogen_file)
print('AutogenSerialization.cpp file has been generated')





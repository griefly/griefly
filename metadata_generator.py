import os
import sys
import re
import json

metadata = {"version" : "v0.1.1",
            "classes" : []}

def extract_macros_params(line, macros_name):
    expr = re.compile(macros_name + "\((.*)\)")
    result = expr.search(line)
    if result:
        extracted = result.group(1)
        return extracted
    return None

def parse_declare_saved_begin(file, class_and_base, filename):
    class_and_base_array = class_and_base.split(',')
    class_name = class_and_base_array[0].strip()
    base_class_name = class_and_base_array[1].strip()
    print("Class header is '" + filename + "'")
    print("Class name is '" + class_name + "'")
    print("Base class name is '" + base_class_name + "'")
    line = file.next()
    class_type = extract_macros_params(line, "DECLARE_GET_TYPE_ITEM")
    if class_type:
        print("Class type '" + class_type + "'")
    else:
        raise ParseError("Next line after 'DECLARE_SAVED' macros does not contain 'DECLARE_GET_TYPE_ITEM' macros")

    variables = []
    while True:
        line = file.next()
        result = extract_macros_params(line, "KV_SAVEBLE")
        if result:
            variables.append(result.strip())
            continue
        result = extract_macros_params(line, "ADD_TO_TYPELIST")
        if result:
            if result.strip() != class_name:
                raise ParseError("'ADD_TO_TYPELIST' does not match 'DECLARE_SAVED'")
            break

    print("Class variables:")
    print(variables)

    class_entry = {"class" : class_name,
                   "header" : filename,
                   "base_class" : base_class_name,
                   "type" : class_type,
                   "variables" : variables}

    metadata["classes"].append(class_entry)
      
def parse_file(fullpath, filename):
    file = open(fullpath, 'r')
    for line in file:
        result = extract_macros_params(line, "DECLARE_SAVED")
        if result:
            parse_declare_saved_begin(file, result, filename)      

def path_equal(path1, path2):
    return os.path.abspath(path1) == os.path.abspath(path2)

for directory, directory_names, filenames in os.walk("Sources"):
    for filename in [f for f in filenames if f.endswith(".h")]:
        fullpath = os.path.join(directory, filename)
        if not path_equal(fullpath, "Sources\Macroses.h"):
            print("Parse file '" + fullpath + "'")
            parse_file(fullpath, filename)

json_data = json.JSONEncoder().encode(metadata)
with open("metadata.json", "w") as json_file:
    json_file.write(json_data)

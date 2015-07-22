import os
import sys
import re

def extract_macros_params(line, macros_name):
    expr = re.compile(macros_name + "\((.*)\)")
    result = expr.search(line)
    if result:
        extracted = result.group(1)
        return extracted
    return None

def parse_declare_saved_begin(file):
    for line in file:
        result = extract_macros_params(line, "DECLARE_GET_TYPE_ITEM")
        if result:
            print(result)
            break

def parse_declare_get_type_item_begin(file):
    for line in file:
        result = extract_macros_params(line, "DECLARE_SAVED")
        if result:
            print(result)
            break   
        
def parse_file(fullpath):
    file = open(fullpath, 'r')
    for line in file:
        result = extract_macros_params(line, "DECLARE_SAVED")
        if result:
            print(result)
            parse_declare_saved_begin(file)
        result = extract_macros_params(line, "DECLARE_GET_TYPE_ITEM")
        if result:
            print(result)
            parse_declare_get_type_item_begin(file)        

def path_equal(path1, path2):
    return os.path.abspath(path1) == os.path.abspath(path2)

for directory, directory_names, filenames in os.walk("Sources"):
    for filename in [f for f in filenames if f.endswith(".h")]:
        fullpath = os.path.join(directory, filename)
        if not path_equal(fullpath, "Sources\Macroses.h"):
            print("Parse file '" + fullpath + "'")
            parse_file(fullpath)

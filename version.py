from __future__ import print_function

import sys

if len(sys.argv) < 2:
    print("Version argument is missing!")
    exit(-1)

with open("Version." + sys.argv[1] + ".cpp", "w") as autogen_file:
    print('#include "Version.h"', file = autogen_file)
    print('', file = autogen_file)
    print('const char* get_game_version()', file = autogen_file)
    print('{', file = autogen_file)
    print('    static const char* const DEFINED_VERSION = "' + sys.argv[1] + '";', file = autogen_file)
    print('    static const char* const UNKNOWN = "v0.0.0-unknown";', file = autogen_file)
    print('    static const char* const VERSION', file = autogen_file)
    print("        = (DEFINED_VERSION[0] == '\\0') ? UNKNOWN : DEFINED_VERSION;", file = autogen_file)
    print('    return VERSION;', file = autogen_file)
    print('}', file = autogen_file)

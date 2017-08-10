import sys

if len(sys.argv) < 3:
    print("Version argument or system info argument is missing!")
    exit(-1)

print("Generating 'Version.cpp'...")

with open("Version.cpp", "w") as autogen_file:
    print("""
#include "Version.h"

const char* GetGameVersion()
{{
    static const char* const DEFINED_VERSION = "{game_version}";
    static const char* const UNKNOWN = "v0.0.0-unknown";
    static const char* const VERSION = (DEFINED_VERSION[0] == '\\0') ? UNKNOWN : DEFINED_VERSION;
    return VERSION;
}}

const char* GetBuildInfo()
{{
    static const char* const INFO = "{build_version}";
    return INFO;
}}

const char* GetQtVersion()
{{
    return QT_VERSION_STR;
}}""".format(game_version=sys.argv[1], build_version=sys.argv[2]), file=autogen_file)
    # PyCharm inspection may highlight 'file=autogen_file' with typecheck warning.
    # That's a well known issue, see https://youtrack.jetbrains.com/issue/PY-23288

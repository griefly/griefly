Griefly
===========

[![Build Status](https://travis-ci.org/griefly/griefly.svg?branch=master)](https://travis-ci.org/griefly/griefly)
[![Coverage Status](https://coveralls.io/repos/github/griefly/griefly/badge.svg?branch=master)](https://coveralls.io/github/griefly/griefly?branch=master)

It is **Griefly: Yet Another Space Station Remake**.

Dependencies
-------------
1. Some C++ compiler (it works at least with compiler from **Visual Studio 2012** and **g++**)
2. Qt5 (e.g., **Desktop Qt 5.4.0 MSVC2012 OpenGL 32bit**). Qt Creator is optional, but desired
3. SFML 2.3
4. libpng
5. Python 2.7
6. Go (https://golang.org/dl/)
7. Git
8. CMake 2.8

How to build everything on Windows
-----------------------------------
_If you have any issues during the build process please report them_   

1. All tools and libs from the depencies list should be available on your machine
2. Open CMakeLists.txt in Qt Creator  
   Open Project -> Select CMakeLists.txt -> Select build folder -> Run CMake _(for the first time)_
3. Environment variables  
   In the opened project: Projects _(left column in Qt Creator)_ -> Build Environment  
   It is needed to set environment variables for libs and tools.
   Mine setted as:  
  `Set GIT_PATH to C:\Program Files (x86)\Git\bin\git.exe`  
  `Set GOFMT_PATH to C:\Go\bin\gofmt.exe`  
  `Set GO_PATH to C:\Go\bin\go.exe`  
  `Set PNG_INCLUDE_DIR to C:\Users\Kremius\Documents\ExternalLibs\lpng1612`    
  `Set PNG_LIBRARY to C:\Users\Kremius\Documents\ExternalLibs\lpng1612\projects\vstudio\Release\libpng16.lib`    
  `Set PYTHON_PATH to C:\Python27\python.exe`  
  `Set SFML_AUDIO_LIBRARY to C:\Users\Kremius\Documents\ExternalLibs\SFML-2.3\lib\sfml-audio.lib`  
  `Set SFML_INCLUDE_DIR to C:\Users\Kremius\Documents\ExternalLibs\SFML-2.3\include`  
  `Set SFML_LIBRARY to C:\Users\Kremius\Documents\ExternalLibs\SFML-2.3\lib\sfml-main.lib`  
4. Rerun CMake with param `-DCMAKE_BUILD_TYPE=Release` for Release verison.
5. Build the project.  
   Client executables will appear in the `Exec` folder, and the server executable will appear in the `griefly-server` folder.  
   Client executables depend from various dlls (Qt, SFML), so it is needed to manually place them to the `Exec` folder.

**Note:** It is supposed to perform build from an active git repository (`git describe --tags --abbrev=4` will be called during the process).

How to build everything on Linux
--------------------------------

1. Install dependencies. Look at `travis-get-deps` rule in Makefile for clues.
2. `make`. Built project will be placed under `Exec` directory.
3. `./build_go_server.sh`

**Note:** It is supposed to perform build from an active git repository (`git describe --tags --abbrev=4` will be called during the process).

How to start server
-------------------

Just run the executable from directory `griefly-server`.

How to run game without launcher
--------------------------------

Pass those command line parameters to `KVEngine` or `KVEngine.exe`:

First (master) client:  
`mapgen_name=brig_small.gen -auto_connect login=<admin login> password=<admin password>`  
where `login` and `password` params values should match values in an auth database (by default `griefly-server/db/auth.json`)  
Other clients:  
`-auto_connect login=Guest`

You can also specify game host by parameter `ip=game_host_address`. By default it connects to localhost.

Codestyle
----------
See `CODESTYLE.md` file.

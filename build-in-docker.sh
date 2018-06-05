#!/bin/bash

set -euo pipefail

# Build
cd /app
./make.sh -DBUILD_TESTS=ON -DBUILD_COVER=ON -DCMAKE_BUILD_TYPE=Debug

# Tests
cd /app/exec
export DISPLAY=:99
Xvfb $DISPLAY -ac -screen 0 1024x768x8 &
sleep 3
./KVEngineTests

# Coverage
cd /app
lcov -c -d . -o coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --remove coverage.info '/opt/*' --output-file coverage.info
lcov --remove coverage.info 'tests/*' --output-file coverage.info
lcov --remove coverage.info 'build/*' --output-file coverage.info
lcov --list coverage.info
if [[ -z ${COVERALLS_TOKEN+x} ]]; then exit 0; fi
coveralls-lcov --repo-token=${COVERALLS_TOKEN} coverage.info || true;

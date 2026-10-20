#!/bin/sh
# (development) builds, then lists the compile errors of this build only
cd "$(dirname "$0")/../.."
rm -rf build/n3ds/compile
sh port/n3ds/build-docker.sh > build/n3ds-last-build.log 2>&1
tail -2 build/n3ds-last-build.log
grep -rh "error:" build/n3ds/compile 2>/dev/null | sed 's/^.*\/work\///' | sed 's/:[0-9]*:[0-9]*:/:/' | sort | uniq -c | sort -rn | head -${1:-40}

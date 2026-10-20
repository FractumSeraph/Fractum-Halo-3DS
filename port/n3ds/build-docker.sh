#!/bin/sh
# Builds Halo3DS in the toolchain image (port/n3ds/docker), with the
# clean-room Xbox SDK declarations of port/n3ds/xdk in place of the SDK's
# headers. Run from the repository's root; any arguments go to build.py.
# The result is build/n3ds/Halo3DS.3dsx.
set -e
cd "$(dirname "$0")/../.."
if ! docker image inspect halo3ds-build >/dev/null 2>&1; then
	docker build -t halo3ds-build port/n3ds/docker
fi
MSYS_NO_PATHCONV=1 docker run --rm -v "$(pwd -W 2>/dev/null || pwd):/work" -w /work halo3ds-build \
	python3 port/n3ds/build.py --devkitarm /opt/devkitpro/devkitARM --clang clang \
	--libctru /opt/devkitpro/libctru --citro3d /opt/devkitpro/libctru --xdk port/n3ds/xdk \
	--picasso /opt/devkitpro/tools/bin/picasso --packer /opt/devkitpro/tools/bin/3dsxtool \
	--jobs 8 --output build/n3ds "$@"

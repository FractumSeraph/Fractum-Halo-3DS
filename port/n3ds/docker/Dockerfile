# The Halo3DS build's toolchain in one image: devkitPro's devkitARM, libctru,
# citro3d, picasso and 3dsxtool, with Debian's Clang for the engine's units
# (port/n3ds/build.py). Build: docker build -t halo3ds-build port/n3ds/docker
FROM devkitpro/devkitarm:latest
RUN apt-get update && apt-get install -y --no-install-recommends clang lld && rm -rf /var/lib/apt/lists/*

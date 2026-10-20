# Halo3DS

Halo 3DS is a port of Halo Combat Evolved from the OG Xbox using recently decompiled source code from the OpenCE project!
It supports all models of the 3DS family including both the OG and New models as well as 2DS models. 
This project also aims to support most of the 3DS's native features including 3D mode and 3DS wireless multiplayer!  

## Playing

You will need a modded 3DS/2DS console, a FAT32 SD card with at least 4 GB of free space and your own game files.
Currently only the 1.0 Halo CE ISO is supported but support for others is being looking into. PC/MCC
maps are not supported.

## Setup

For command-line setup on **Linux, macOS or Windows**, see the
[cross-platform CLI guide](docs/SETUP-CLI.md). The CLI can be built from source
with .NET SDK 10; its packaged builds include the runtime. The Windows graphical
setup instructions follow.

1. Download the setup tool for **Windows 10/11** or **Windows 7** from the
   [releases page](https://github.com/Generalkidd/Halo3DS/releases). Both include the game executable. The Windows 7 version is
   also compatible with Wine on Linux and Mac OS. 
3. Open the tool and select your **Xbox ISO/XISO** or **dumped maps folder**.
4. Select your SD card's drive, such as `H:\`, and click **Prepare SD card**.
   Leave the card connected until setup finishes.
5. Safely eject the card, put it back in your console, and launch **Halo3DS**
   from the Homebrew Launcher. The first load may take a while.

Use the English North American retail Xbox release. If you're using a maps
folder, keep its matching `default.xbe` in the parent folder. If setup can't
find it, select it under **Additional options > HUD compatibility file**.
No debug executable is needed.

If setup asks you to dump DSP firmware for sound, do this once on your console:
press **L + D-pad Down + SELECT**, then choose
**Miscellaneous options > Dump DSP firmware** in Rosalina.

## Multiplayer (this branch)

The `multiplayer` branch runs the
[FractumSeraph/OpenCE](https://github.com/FractumSeraph/OpenCE) fork's engine
under this port, with its network code (network version 32: it joins games of
versions 11 to 33):

- **System Link Play** works with the fork's games and servers and with other
  3DS consoles on the same network.
- **Internet play** works the fork's way (its MQTT signalling brokers, hole
  punching and its tunnel; no WebRTC or UPnP on the 3DS). For now a game is
  joined by its invite: put `network_test = "invite:<the invite's 64 hex
  digits>"` under `[debug]` in `config.toml` (the last part of a
  `halo://join/` link); the game joins it at start. A list of public games in
  the 3DS's menus is still to come, as are Custom Edition maps and map
  downloads; [the plan](docs/MULTIPLAYER-PLAN.md) says where each stands.

The console needs its Wi-Fi on. Settings are in `halo-source/config.toml` on
the SD card, written with every setting and its description the first time
the game runs; under `[network]`:

- `address`: this console's IPv4 address, if it should use a particular one
- `broadcast`: where system link announcements go instead of the network's
  broadcast address, for example `"192.168.1.20"` (a PC's address) if the
  console finds no games: some networks and consoles don't pass broadcasts
- `online`: internet play (on by default)

## Updating

In the setup tool, select your SD card and choose **Update the launcher only**.
You can select a newer `Halo3DS.3dsx` under **Additional options** if you want
to keep using an older setup tool.

You can also replace `3ds/Halo3DS/Halo3DS.3dsx` on the SD card yourself.
These updates keep your maps and saved files. Unless a release says otherwise,
you don't need to run the full setup again.

## Reporting problems

Include your console model, build version, mission/map, difficulty, whether 3D
was on, and what happened. For a crash or freeze, power off before removing
the SD card and copy the logs before launching Halo3DS again:

- `halo-source-engine.log` and `halo-source-engine.previous.log` at the SD root
- `halo-source-fatal.log` and `halo-source-stall.log`, if present
- The newest `.dmp` in `luma/dumps/arm11/`, if one was saved

A freeze or return to the Homebrew Launcher may not leave a crash dump.
Check logs before sharing them, and don't include game files or firmware.

## Development

[Developer notes](docs/DEVELOPMENT.md) cover building the game and both setup
tools, the code layout, testing, and details to watch out for when making changes.
For the setup tools, start with the [Visual Studio solution instructions](docs/DEVELOPMENT.md#visual-studio).

## Origin and notices

Based on [bnunu/halo-1](https://github.com/bnunu/halo-1), revision
`8d0c1ff46ef92511e4809fff422bacd94c3b7c67`. The `multiplayer` branch's engine
and its network layer (`port/linux`) are the FractumSeraph/OpenCE fork's,
under the GPL ([LICENSES/GPL-3.0-or-later.txt](LICENSES/GPL-3.0-or-later.txt)),
and so is that branch as a whole; settings are read with
[tomlc17](port/third_party/tomlc17/LICENSE), internet play uses
[kcp](port/third_party/kcp/LICENSE) and [Monocypher](port/third_party/monocypher/LICENCE.md).

See [LICENSE.md](LICENSE.md), the [setup component notices](port/n3ds/setup/THIRD-PARTY.md),
and the licenses for [LZ4](port/n3ds/vendor/lz4/LICENSE) and the
[allocator](port/n3ds/vendor/libctru-allocator/LICENSE.txt).
Original game assets, SDK files and Nintendo DSP firmware are not included.

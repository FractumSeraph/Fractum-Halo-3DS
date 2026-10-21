# Fractum Halo

Fractum Halo, by FractumSeraph, is Generalkidd's [Halo3DS](https://github.com/Generalkidd/Halo3DS)
with the multiplayer of the FractumSeraph/OpenCE fork: system link, internet
play, the server browser, and Custom Edition maps with their downloads (on a
New 3DS). The Homebrew Launcher lists it as **Fractum Halo**. Its file keeps
Halo3DS's name and place, `3ds/Halo3DS/Halo3DS.3dsx`, so Halo3DS's setup tool
still installs and updates it.

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
5. Safely eject the card, put it back in your console, and launch **Fractum Halo**
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
  punching and its tunnel; no WebRTC or UPnP on the 3DS).
- **Online Games**, under Multiplayer, is the server browser: every game on
  the community game list (OpenCE's and ChupathingyCE's servers), with its
  map, game type, players and ping. Up and down (or a tap on the bottom
  screen) pick a game, left and right turn the page, **A** joins it, **X**
  refreshes, **B** goes back. **SELECT** opens the keyboard to join a game
  by its invite (a `halo://join/` link or its 64 digits).
- **Hosting**: **Y** in Online Games creates a game on the internet. Its
  invite is shown on the bottom screen for friends to join; a public game
  (the default, `network.host_public`) is also listed in everyone's browser.
- **Custom Edition maps** (Halo PC's, `<map>@ce`) play on a **New 3DS**
  whose game is given 96 MB of memory (`halo-source-engine.log` says
  `CE MAPS: playable` at start-up, or why not):
  joining a server on one downloads its map from the server (map sharing)
  into `halo-source/maps_ce`, and the game loads it there. Maps you have
  go in `halo-source/custom_maps` or `halo-source/maps_ce`, with Custom
  Edition's `bitmaps.map`, `sounds.map` and `loc.map` beside them (copy
  them from a Halo PC install). A few of their shader effects are not
  drawn yet; [the plan](docs/MULTIPLAYER-PLAN.md) says what is left.

The console needs its Wi-Fi on. Settings are in `halo-source/config.toml` on
the SD card, written with every setting and its description the first time
the game runs; under `[network]`:

- `address`: this console's IPv4 address, if it should use a particular one
- `broadcast`: where system link announcements go instead of the network's
  broadcast address, for example `"192.168.1.20"` (a PC's address) if the
  console finds no games: some networks and consoles don't pass broadcasts
- `online`: internet play (on by default)

If the game crashes, an empty file `halo-source/sync-log.txt` on the SD card
makes the game write each line of `halo-source-engine.log` as it comes, so a
crash loses none of the last ones (slower; delete it afterwards). Luma's
crash dumps are in `luma/dumps/arm11`.

The game list is reached over HTTPS. The game knows Let's Encrypt's root
certificates; for others, put a certificate bundle at
`halo-source/cacert.pem`.

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
the SD card and copy the logs before launching Fractum Halo again:

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
[kcp](port/third_party/kcp/LICENSE) and [Monocypher](port/third_party/monocypher/LICENCE.md), the game list
[Mbed TLS](port/third_party/mbedtls/LICENSE), [qrcodegen](port/third_party/qrcodegen/LICENSE) and
[zlib](port/third_party/zlib/LICENSE).

See [LICENSE.md](LICENSE.md), the [setup component notices](port/n3ds/setup/THIRD-PARTY.md),
and the licenses for [LZ4](port/n3ds/vendor/lz4/LICENSE) and the
[allocator](port/n3ds/vendor/libctru-allocator/LICENSE.txt).
Original game assets, SDK files and Nintendo DSP firmware are not included.

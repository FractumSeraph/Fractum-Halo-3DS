# Multiplayer on Halo3DS: the plan

Halo3DS (this repository) runs Halo on the 3DS: its own renderer, audio,
saves and frontend over the decompiled engine, tested on Original and New
3DS hardware. It is a campaign port: no network, Xbox retail maps only.
The FractumSeraph/OpenCE fork's 3DS port (branch `3ds` of that fork) has
what this one lacks: system link and internet play with the OpenCE and
ChupathingyCE servers (network version 29, the Chupa wire, Delta), Custom
Edition maps and map sharing. The goal is both in one: this port, with
the fork's multiplayer. The fork's merge policy holds: keep every feature
of both; most players joining, maps downloaded on their own.

## What was found (2026-10-20)

- **The build.** Halo3DS builds without the Xbox SDK: the fork's clean-room
  declarations (`port/n3ds/xdk`) stand in for its headers, with three
  constants added, in a Docker image of devkitPro's toolchain and Clang
  (`port/n3ds/build-docker.sh`). The setup CLI runs in .NET's image and
  prepared all 24 maps from a USA Rev 2 disc image (its executable's HUD
  text and loading art verify as the retail files). The main menu runs in
  the Azahar emulator.
- **The engines.** Halo3DS is based on bnunu/halo-1 at 8d0c1ff4, which the
  fork also follows (the fork has that change as a6e7a3a7). From that base,
  Halo3DS changed its engine (`source/`) by some 4,000 lines in small
  edits; the fork by some 78,000 lines (its network protocol, menus, the
  native ports' fixes) and 276 files. A three-way merge of Halo3DS's
  changes onto the fork's engine, file by file with git merge-file: 504 files
  untouched by Halo3DS, 75 merged cleanly, 51 with conflicts (about 120
  hunks, most of them a few lines; ui_widget_event_handler_functions.c has
  24), 3 files added.
- **The network.** Halo3DS compiles all the engine's networking units, but
  never starts the 3DS's socket service, and its XNet functions are stubs.
  The fork's platform layer has the rest: Winsock over BSD sockets
  (posix_net.c, xnet.c, with the Azahar and console socket quirks already
  handled), internet play (p2p.c: STUN, brokers, the tunnel, invites), the
  Delta layer, map sharing, the shared ban lists.

## Phase 1: where it stands (2026-10-20)

The fork's engine runs under Halo3DS's frontend (branch `multiplayer`):
in Azahar the menus, the campaign (The Pillar of Autumn, its scripts and
cutscenes) and a solo Slayer game on Blood Gulch (moving, turning, firing)
run as with Halo3DS's own build.

- **How it is put together.** `source/` is the fork's engine with Halo3DS's
  changes merged in. The fork's port layer is in `port/linux` (headers,
  `game/`, `src/`), reached after this port's own include folders
  (`after_include_dirs` in `port/n3ds/sources.json`); of its units, only
  what the engine needs to link is built so far. The rest are stand-ins in
  `port/n3ds/src/engine_fork_stubs.c`: each returns what the fork's unit
  returns with nothing of its own going on (no network game, no co-op, no
  high-res assets), and the settings are the fork's settings table's
  defaults. `port/n3ds/include/engine_shims` holds include-only stand-ins
  for the headers upstream OpenCE folded together, so Halo3DS's native code
  still finds them; `engine_fork_exports.h` declares the fork's functions
  that Halo3DS's code calls.
- **Halo3DS's speed-ups.** Brought over onto the fork's code: the AI's
  decision deferral (near any player, not only the local ones, since a co-op
  host runs the AI for everyone) and perception scheduling, decorative
  particle thinning, the model part cache with merged opaque draws (a model
  with a bad index in its map falls back to the fork's checked path, which
  reports it), scenery's flags for the native renderer, the collision
  feature cache, collision statistics off unless asked for, and the
  visibility pass without the Xbox's surface lists. Its profiling hooks
  (opt-in defines) are left out.
- **The emulator.** Azahar's software renderer crashes on Blood Gulch's
  first frame (a cache flush of address 0 in the emulator), with Halo3DS's
  own build as well: test with Vulkan (`graphics_api=2`).
- **The license.** Halo3DS's own work is CC0; the fork's engine and port
  layer are GPL-3.0-or-later (`LICENSES/GPL-3.0-or-later.txt`), so this
  branch as a whole is distributed under the GPL.

## Phases

1. **The fork's engine under Halo3DS.** `source/` becomes the fork's, with
   Halo3DS's engine changes merged onto it (the conflicts resolved by hand,
   keeping both sides' intent), built as a 32-bit engine (`HALO_N3DS`, not
   the fork's 64-bit `HALO_3DS` layout: the 3DS's pointers are 32-bit, and
   Halo3DS keeps the Xbox's layout native). The engine's calls into the
   fork's port layer (`port:` changes: settings, logs, map families, the
   network test) are given what they need, from port/linux where it is
   platform-neutral. Done when this port runs as it does now (menus, the
   campaign, solo Blood Gulch) on the fork's engine.
2. **System link.** The socket service started; the fork's Winsock and XNet
   layer in place of the stubs; a game hosted on a PC (the fork's Windows
   dedicated server) joined from the 3DS in Azahar, on loopback. Then the
   3DS hosting.
3. **Internet play.** p2p.c, the brokers, STUN and the tunnel; joining a
   public server by its invite (an empty game, leaving soon); the Delta
   layer and network version 29 as the fork has them, the wire unchanged.
4. **Maps.** Whether a server accepts a map converted by the setup (its
   header and checksum); map sharing's downloads, which this port must
   convert on the console (the setup does it on a PC now); Custom Edition
   maps, which this port's loader does not read (the fork's loader and its
   memory scheme, or a conversion like the setup's).
5. **Hardware.** Each phase's build tried on a New 3DS; the Original 3DS's
   memory checked for the network buffers.

The fork's own 3DS port (branch `3ds`) stays as it is until this one has
what it has.

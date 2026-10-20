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

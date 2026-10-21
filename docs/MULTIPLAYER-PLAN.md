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

## Phase 2: where it stands (2026-10-20)

System link works in Azahar, both ways, with the fork's Windows game
(network version 29) on the same PC: the 3DS on 127.0.0.2, the PC's game on
127.0.0.3, each with `network.broadcast` set to the other's address.

- **3DS hosting.** The 3DS hosts Slayer on Blood Gulch from System Link Play;
  the PC's game (`HALO_NETWORK_TEST=join`) finds it, joins, both load and
  play, each sees the other move; the 3DS returns to its menu when the PC's
  game leaves.
- **3DS joining.** The PC's game hosts (`HALO_NETWORK_TEST=host:bloodgulch`);
  the 3DS lists it, joins, plays.
- **What it took.** The fork's Winsock and XNet layer (`port/linux/src/xnet.c`,
  `posix_net.c`) in place of Halo3DS's offline stand-ins, the engine's Winsock
  calls under the fork's names, the socket service started at boot, the
  transport started and the server's key registered (the fork's builds
  define `xbox`; this one does not, so those two places name `HALO_N3DS`),
  the network's part of each frame in Halo3DS's frame loop (it had none),
  the fork's network game units (distributed netcode, objects, damage,
  actors, co-op, fair play, vote kick, team change, the network test), and
  its settings (`port_config.c`, `halo-source/config.toml`).
- **Still stand-ins.** Internet play (`p2p.c`: the identifier and hardware id
  are random for each run until then), voice, map sharing and downloads,
  Custom Edition maps, the PC menus and pointer, Discord.
- **The emulator.** Azahar runs the game at about a third of the console's
  speed, so a host there finds a PC client's clock running fast and refuses
  its players' predictions (logged); on a console at full speed that should
  not happen. Azahar's socket service refuses `SO_BROADCAST` (tolerated):
  whether a console sends broadcasts without it is for the hardware test,
  with `network.broadcast` as the way around it.
- **Network version.** Now the fork's latest (below).

## Network version 32 and Phase 3: where they stand (2026-10-20)

- **The fork's latest.** The fork's changes from 214407325 (the old 3DS
  branch's base, network version 29) to fork/main 8ab6b1324 (network version
  32, joining hosts 11 to 33) are merged in (f943fdf): 163 files merged, 33
  added, 51 empty headers removed, seven conflicts resolved keeping both
  sides. Its new units are built (network rules and aim assist, text chat
  and its filter, bots, co-op pickups and vehicles, the latency meter, the
  overshield effect, the passenger camera, structure culling, model
  detail). The model part cache takes its `display.model_detail` rules.
  To follow the fork again: `port/n3ds/tools/fork_update.py`'s three-way merge from the
  last fork commit taken (now 8ab6b1324) to the new one.
- **Internet play.** The fork's p2p layer and Delta are built (9a3b288):
  STUN, the MQTT brokers (its built-in list: halovps.fractumseraph.net
  first), the tunnel with kcp, Monocypher's signatures; no WebRTC (the
  fork's browser build's stand-in), no UPnP, no Discord. In Azahar the 3DS
  joined two empty public dedicated servers by their invites: one's lobby
  (a team game waiting for players) and the other's game (Slayer on
  Derelict, 123 ms), and left.
- **Still to do.** Hosting on the internet from the 3DS
  (its games get an invite and are listed, untested); a version 32 PC game
  to test 3DS-hosted system link against (the PC game here is version 29,
  which joins hosts up to 30 only); voice; the fork's deterministic maths
  (musl's, which its lockstep games need; the distributed netcode corrects
  differences); then Phase 4's maps.

## The server browser (2026-10-20)

The 3DS build is the fork's ChupathingyCE edition (`HALO_GAME_BROWSER`, as its
released games are built: Online Games, Bomb, Delta Peer with ChupathingyCE
hosts). Multiplayer > ONLINE GAMES is the fork's own browser screen for the
Xbox menus (`browser_screen.c`), listing the game list's games
(halo.milenko.org over HTTPS: OpenCE's brokers' games and ChupathingyCE's
announced ones); A joins one through its invite. In Azahar it listed 47
servers and joined the empty dedicated "[C] Slayer" from the list and played
(Rat Race, 87 ms).

- **Drawing.** The fork draws its screens over the menus with an OpenGL
  overlay (`ui_overlay.c`); on the 3DS `port/n3ds/src/engine_ui_overlay.c`
  draws the same calls in the menus' own pass: the game's screen quads for
  shapes (square corners, gradients in bands), the menus' fonts scaled for
  text. Pictures (the site's JPEG map art), platform logos and glows are not
  drawn; Halo's own map pictures are.
- **HTTPS.** Mbed TLS (the fork's copy and defines; entropy from the PS
  service; `engine_mbedtls_net.c` for its sockets and clock), Let's
  Encrypt's ISRG roots built in after `sdmc:/halo-source/cacert.pem`.
- **Touch, invites, hosting** (cc9e9da). The bottom screen is the menus'
  pointer (the fork's `touch_menu.c`: tap to click, drag to scroll; its
  320x240 is the menus' 640x480). SELECT in Online Games opens the system's
  keyboard for an invite, joined as a picked game is. A game hosted on the
  internet (Y) shows its invite on the bottom screen. Tested in Azahar: the
  fork's latest Windows game (network 32, built from fork/main in
  `build/opence-v32`) joined a private 3DS-hosted game by that invite and
  played; a typed invite joined a public server. (Azahar's keyboard needs
  its built-in applets: `lle_applets=false`.)
- **Then:** Custom Edition games joinable (Phase 4, below).
  The New 3DS is the target; the Original 3DS is not checked for now.

## Phase 4: Custom Edition maps and map downloads (2026-10-20)

Halo PC maps (`<map>@ce`) load and play on the New 3DS (e1548b3, a08b2a5),
and a joined game's map is downloaded (3e8dea4).

- **Memory.** A Halo PC map is linked to its 23 MB tag cache at 0x40440000,
  which a 3DS program cannot map. A block of the console's memory (31 MB:
  the tag cache and the fork's 8 MB conversion room) stands for it
  (`port/n3ds/src/engine_ce_cache.inl`). Only a New 3DS has the room, so
  `platform_ce_tag_cache_ready` is set on one at start-up (`engine_main.c`),
  if the game was given 96 MB or more (Azahar gives 96); else the menus say
  why, and the log's `CE MAPS:` line gives the size.
- **Loading.** The tags are read into that block. The fork's conversion
  then runs on it as on its own builds (resource maps' indexed tags,
  repairs, gbxmodels made models, functions, shaders, HUD, names), with
  the map's addresses translated (`source/cseries/xbox_address.h`). Next
  every pointer in it is made native, walking Halo3DS's tag layouts
  (`engine_ce_relocate.c`, tables generated by
  `port/n3ds/tools/ce_schemas.py`; a PC test in
  `port/n3ds/tests/ce_relocate_test.c`). From then on Halo3DS's renderer,
  textures and sound read it as an Xbox map.
- **What the map needed.** Bitmaps are marked cached and read from the
  file their tag names (bitmaps.map or the map). They are in Halo PC's
  layout (`N3DS_BITMAP_PC_LAYOUT_FLAG`: rows not swizzled, cube faces
  not padded). A model's multipurpose map has its channels moved to the
  Xbox's order. Sounds are read from sounds.map or the decoded Ogg
  Vorbis file (`z:\ce_sounds.pcm`, `sdmc:/halo-source/z/`); six of the 24
  DSP voices are 16-bit PCM. A BSP's and a model's converted vertices go
  in the tag cache's room. ui.map's button icons (32 MB to inflate) are
  declined: HUD messages draw no controller buttons.
- **Downloads.** The fork's map sharing, its HTTPS downloader and its
  download screen are built. Its engine units' `fopen`, `remove` and
  `rename` take Xbox paths (`engine_prefix.h`, as the fork's stdio.h
  shadow does). A download goes into `halo-source/maps_ce`.
- **Tested in Azahar.** ChaosGulch@ce local (moving, firing, sounds),
  Blood Gulch unchanged. On loopback system link, a v32 PC host: the 3DS,
  without the map, downloaded it from the host (13.9 MB, byte-identical),
  loaded it and played. Over the internet: FractumSlayer (concealed@ce,
  v32, 17 MB) and Haloss1 ArenaCE (delta_ruined@ce, v29, 29 MB), each
  empty, joined by invite, the map shared by the server, played briefly
  and left. Map sharing ran at about 400-500 KB/s in the emulator.
- **Not yet.** Chicago shaders of types or flags no Xbox map uses are not
  drawn (logged once each, `CHICAGO PC:`); other shader kinds may still
  stop on what Halo PC maps alone use. The HTTPS repositories'
  downloads (for a host that does not share its map) are built but
  untested. No BitTorrent. HaloMD maps are not tried. The hardware run.
- **Test files.** `build/sdcard/halo-source/custom_maps` holds copies of
  ChaosGulch and Custom Edition's resource maps, for the emulator only;
  downloads made in tests are deleted after each run.

## Phase 5: on a New 3DS (2026-10-21)

The first runs on a New 3DS (124 MB for the game; `CE MAPS: playable`):

- **Found and fixed.**
  - The console's socket service refuses a bind to port 0, which Azahar
    takes, so internet play's tunnel did not open. Every bind to port 0 now
    picks a dynamic port (`posix_socket_bind`, 55ba0b4). The fork's own 3DS
    port does this for the tunnel alone.
  - Socket buffers are capped at 64 KB (4b0d745). The game asks 1 MB per
    socket, and all of them share socInit's 1 MiB.
- **The Online Games crash, found.** The console's dump (build
  a442445a529a) pointed at `draw_unicode_string_compute_bounds`. The
  3DS overlay's text measure passed it a NULL cursor rectangle, which it
  writes: a write to address 2. Azahar logs such an access and goes on;
  the console stops. Fixed, with a rectangle of its own.
- **Azahar's log catches them.** Every memory access the console would
  stop on is in Azahar's `user/log/azahar_log.txt` ("unmapped Write16 ...
  at PC ..."). After the fix, these runs logged none: the browser (paged,
  invite typed, a private game hosted from the menus), ChaosGulch@ce and
  Blood Gulch played, and a loopback join that downloaded its map.
- **The map list.** With Custom Edition built in, Halo3DS's filter of
  the Xbox maps on the SD card had been compiled out: missing maps were
  listed. `ui_map_list.c` now lists only those `n3ds_engine_map_installed`
  finds, then the Custom Edition maps (ChaosGulch [CE] has its picture and
  description). An empty list is refused when picked. The first pick of a
  Custom Edition map hashes it for map sharing: about 30 s in Azahar, a
  few on a console, and kept afterwards.
- **Checked in Azahar since.** The screen opens with internet play off, with
  the 3D slider up (two views), with the game list unreachable or its name
  unknown, and with the clock in 2010 (the certificate refused, said in the
  log). None stops the game.
- **Looked over and clean.**
  - Every thread goes through the 256 KB-stack wrapper.
  - There are no 64-bit atomics.
  - No byte buffer is cast to a wider type in the network, browser or map
    sharing code.
  - The renderer gives the GPU only its own linear buffers, never tag
    memory (a Halo PC map's space is ordinary heap).
  - The fork's socket layer already has its 3DS changes. libctru defines
    `SO_BROADCAST` as 0, so the one refusal in the log is expected.
- **Speed.** Online Games took 43 ms a frame to draw in Azahar, against 6 ms
  for the main menu. It draws about 90 strings and 1,400 glyphs, and each
  glyph was a draw call of its own. Now:
  - A string's glyphs are drawn in one call (`engine_text.c`,
    `n3ds_gpu_text_draw_quads`).
  - The overlay looks its fonts up once per map
    (`n3ds_engine_tag_generation`).
  - Gradients use a band per 32 units.

  The screen now takes 24 ms in Azahar; the console runs about three times
  Azahar's speed.
- **Stuck on the loading screen** (the console, now and then; one boot
  reached the menu at once, one after 37 s, one never). All of the game's
  threads share one session with the 3DS's socket service, which takes one
  call at a time. A call that waits there (a DNS lookup, a connect, a poll
  with a timeout) holds up every other thread's socket calls, the main
  thread's too. Azahar's service does not, which is why it never showed
  there. On the 3DS now:
  - Every poll of the socket layer and the HTTPS client waits a slice at
    a time: a poll that does not wait, then 4 ms of sleep
    (`posix_3ds_poll`).
  - The HTTPS client connects without waiting in the service (10 s at
    most).
  - DNS lookups are made one at a time, kept for the run (a failure for a
    minute), skipped while the console has no address, and logged when
    slow.

  Whether this was the hang is for the next console run to say.
- **The freeze before the main menu, found** (the stall log: frame 1,
  `audio`). The first menu frame's music waited for the SD card's lock
  (`n3ds_files_lock`) while the start-up filesystem checks held it: 15.6 s
  in Azahar, and far longer with your 512 GB card. Two causes:
  - The checks asked for the card's free space (`statvfs`), which the 3DS
    counts by reading the card's whole allocation table: minutes on a
    large card. It is no longer asked (`n3ds_disk_space`); plenty is taken
    to be free, as saves are a few KB.
  - The checks' thread ran at the game thread's priority on its core, and
    threads of one priority there take turns only when one waits. It took
    the lock again after each of its slow calls before the game's thread
    could. It and the two profile-saving threads now run one step lower
    (`create_thread` flag 2), so the game's thread gets the lock at once.

  Azahar now reaches the menu with no stall.
- **Engine threads below the game thread.** `n3ds_thread_create` now
  puts every engine thread one step below the game thread unless it asks
  to be above it. Map sharing's hashing thread asked for the game
  thread's own priority, and hashing a map (and its 123 MB bitmaps.map)
  kept the game thread from running: a pick of a Custom Edition map stood
  still for 30 s in Azahar.
- **Start-up's SD work cut.**
  - The 29 default gametypes were emptied and written again every boot
    (14.7 s in Azahar); a checksum file (`defaults.sum`) beside them now
    leaves them when unchanged. The checks now take 3.6 s, most of it
    the one scan of the saves every boot needs.
  - The checks log when they end ("filesystem checks ... done"): the
    menus take input only from then on.
- **Hashes kept on the SD card.**
  - A map's version hash: `halo-source/map_hashes.cache`
    (`map_file_hash.c`).
  - Map sharing's file hashes: `halo-source/share_hashes/`
    (`map_share_files.c`), checked against the file's pieces when read.

  Each is keyed by the file's path, size, time and its first and last
  4 KB (the 3DS's files may give no time). A map is read whole once, not
  each run. Hosting ChaosGulch@ce now gives its version 6 s after the
  network game is set up (about 30 s before).
- **The menus' map list, 5.8 s to 1.1 s** (Azahar; logged when over
  250 ms).
  - It no longer lists the data folder (the setup's 24 Xbox maps) for the
    Halo PC families, which it did six times a refresh, a directory entry
    at a time (about 28 ms each there).
  - The Xbox maps' installed checks are kept for the run.
  - A map's header is read once a run (`map_families.c`; map files are
    never changed in place).

  Opening the list again takes under 250 ms.
- **Profiles.** The loaded map's name was "" on the 3DS, so the main menu's
  own functions (making, editing, deleting a profile) were refused as a
  foreign map's. `scenario_tags_load` sets it now.
- **A test of the socket service as the console has it.** With
  `halo-source/soc-serial.txt` present, every socket call is made under one
  lock (`engine_soc_serial.c`, wrapped at link time: `port/n3ds/build.py`).
  A call that holds the service more than a second is logged.
- **The stall watcher** (`halo-source-stall.log`) now counts a logged line
  as progress, so it watches start-up and map loads as well as frames.
  After 5 s without progress, and every 30 s it lasts, it writes the last
  24 lines logged.
- **To find a crash.**
  - An empty `halo-source/sync-log.txt` writes the engine log line by line.
  - Luma's dump is in `luma/dumps/arm11`. Its PC is mapped with the
    matching ELF, which `build/symbols/Halo3DS-<sha256 prefix>.elf` keeps
    for each build handed over. The last 96 bytes of the dump's code end
    at the PC, and searching the ELF's `.text` for them confirms the build.

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

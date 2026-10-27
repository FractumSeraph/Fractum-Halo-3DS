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
  last fork commit taken (now 4a752e42f) to the new one.
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
  downloads (for a host that does not share its map) work since the
  map repositories' root certificates were added (below). No BitTorrent. HaloMD maps are not tried. The hardware run.
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
- **HTTPS map downloads.** The map repositories (maps.halonet.net and
  its mirrors) are certified by Google Trust Services (WE1 under GTS Root
  R4), which the 3DS's built-in roots lacked: "certificate is not
  correctly signed by the trusted CA". GTS Root R1, R3 and R4 are built
  in now (`engine_tls_roots.h`). Tested with map sharing turned off
  (`network.map_download = false`, `map_downloads = "always"`): joining
  Haloss1 Hugeass downloaded its 23.5 MB zip, unpacked hugeass.map
  (59 MB, size and CRC checked, the host's version) into maps_ce, and
  joined again. That join reached the host but did not complete: the
  server most likely still held the 3DS's first session.
- **Hosting a Custom Edition game at full speed.** Delta Peer's host
  worked out the game's map's identity every frame (`host_map`: the map
  found in its folders, its hash's file stamp), 133 ms a frame in Azahar.
  Hosting ChaosGulch@ce (a local game too) ran at about 7 frames a second
  there. It is worked out once a map now: the frame's network part takes
  1.5 ms, and the game runs at 30.
- **The Original 3DS, in Azahar.** In its old-model mode (`is_new_3ds =
  false`) the game says so ("MODEL: original 3DS/2DS"), refuses Halo PC
  maps, and plays Blood Gulch, slowly at the emulated 268 MHz. The build
  with the Original 3DS's memory (`build.py --original-memory`: a 24 MB
  heap and 32 MB of linear memory) loaded Blood Gulch with 16 MB of the
  heap in use and 12.5 MB of linear memory free, and listed Online Games'
  64 servers with 20 MB in use. Not tried on an Original 3DS.
- **Waits on engine threads.** The profiles' saves wait for their writing
  thread with empty loops (`while (!thread_has_exited(...)) {}`), which on
  one core never let that thread run: a save while the last one was still
  writing would have stopped the game. `thread_has_exited` sleeps 1 ms when
  the thread is still running.
- **Still to do.** Custom Edition HUD messages draw no controller buttons:
  the fork reads the icons from the Xbox's compressed ui.map, and the
  setup's is uncompressed (33.5 MB). Its tag data and the icons' pixels
  could be read from it instead, once Halo3DS's HUD is shown to draw them.
- **Socket options the console refuses.** A New 3DS refuses SO_SNDBUF
  (errno 109), which `debug.txt` showed as "WSAENOPROTOOPT" every game.
  libctru defines SO_BROADCAST as 0, which the service refuses (EINVAL).
  `posix_net.c` skips both on the 3DS: the sockets keep their default send
  buffer, and broadcasts go out without the option (system link search was
  checked in Azahar).
- **A Halo PC map that fails to load** after its 31 MB tag space is
  allocated gives the space back (`ce_tags_load_failed`). The Xbox map
  loaded next has that memory again. A map whose model data can't be read,
  or can't get memory, is refused rather than drawn with garbage vertices.
- **Cube maps** (glass, water and generic model reflections) no longer stop
  the game on one this can't read, or on a map with more than eight. The
  reflection isn't drawn, and the engine log says so once. A Halo PC map's
  cube faces are read in its layout: levels in order, six faces each.
  Nothing is written to the SD card in play any more.
- **The map list's kept headers** are forgotten when map sharing adds or
  deletes a map. Hosting retries the map's identity every 3 s while it's
  not found.
- **Fractum Halo.** The build embeds an SMDH (`port/n3ds/build.py`,
  smdhtool): title "Fractum Halo", author "FractumSeraph", and the logo
  (a moon over a teal ring) at 48 pixels (`port/n3ds/art/icon-48.png`,
  made from `port/n3ds/art/logo-512.png`, 2048 px as given). The loading and stop screens say Fractum Halo.
  The file name and folder stay Halo3DS's, for its setup tool.
- **Custom Edition maps played through in Azahar** (20 from HaloWeb's
  assets): what stopped them is fixed.
  - Plasma (shields, teleporters): a Halo PC map's 3D textures aren't
    swizzled, and are often A8R8G8B8 (format 11) where the Xbox's are A8Y8.
    Both are read, at the first mip level that fits the 64 KB buffer (16³
    for format 11). Anything else skips the plasma, logged once.
  - A texture that fails to bind (a full cache, or a format not handled)
    skips that draw. Eleven asserts stopped the game on it.
  - Water with values out of range, a zero normal, or data outside the BSP
    is pinned or skipped.
  - A big map's BSP vertices can go in heap blocks when the tag cache's
    free room runs out (`ce_bsp.c`, coldsnap). `n3ds_cache_resolve` now
    accepts those blocks for the BSP's view (`ce_bsp_vertex_memory_holds`).
    A material whose vertices are still nowhere known is left out and
    logged ("MISSION RENDER: ... left out"). Before, the whole render failed.
  - A tag block or tag that isn't word-aligned, or that points outside the
    space, is made empty. The console's VFP faults on unaligned floats,
    and nothing is mapped at the map's own addresses.
- **A stop now shows on the bottom screen.** A failed assert shows the file,
  line and check, mutes the sound and waits for START before leaving.
  Before, the game quit with nothing on screen.
- **The socket service closes safely.** After `n3ds_soc_close` begins,
  every socket call fails with ENETDOWN, and `socExit` waits up to 2 s for
  calls in progress. Before, a network thread's poll after `socExit` read
  freed memory (null), which Azahar logged and a console would crash on.
- **Review pass (2026-10-21, evening).** Two code reviews of the port
  layer and the renderer; what they found, fixed:
  - *Fewer file calls on a large SD card.* `CreateFileA` opens without a
    stat first (a map looked for in each folder cost two calls a miss).
    Directory listings take the entry's kind from `d_type`, no stat each
    (sizes are 0; `network_recording.c` asks for the ones it needs). The
    map list keeps each map's details, description and picture presence
    for the run (`ui_map_list_forget`, from `map_families_headers_forget`).
    `map_file_hash_prefix` looks in its cache by one stat before opening
    the file (eleven calls). A profile's save folder is remembered
    (`engine_saves_platform.c`), not found by reading every folder.
  - *Hashing a map to host it* (`map_share_map_hash`, on the game thread:
    30 s for a 14 MB map) now reads and writes the SD card's hash cache
    inside `hash_job_run`, so it happens once a file, not once a run, and
    the catalog's cache reads are the hashing thread's.
  - *A joined game's map identity* is asked twice a second while its hash
    is pending, not every frame (twenty file calls each).
  - *A map download* holds the TLS lock only while it opens; the game list
    and anchor pings no longer wait minutes behind it.
  - `thread_has_exited` sleeps only when polled in a tight loop (it cost
    1 ms a frame while a hash or connect ran). The DNS cache evicts when
    full instead of looking a 33rd name up every time. `socExit` is
    skipped, the service left open, if a thread is still in it after 2 s.
  - *Renderer, for Halo PC maps:* BSP materials past 512 (now 1024), out
    of GPU memory, with a bad index or a non-finite vertex are left out and
    logged ("NATIVE BSP: ... not drawn"), not an exit. Generic (`sotr`)
    shaders this can't draw, numeric chicago shaders without animation,
    particle secondary maps of other blends, meters of tint mode 2, decals
    without a texture on the CPU path, and parts without vertices or
    strips are skipped the same way. A stage period of 0 means no
    animation. Model vertices the conversion put on the heap are readable
    by the renderer (`ce_models_vertex_memory_holds`), as BSP ones were.
  - *CE sounds in sounds.map* were refused by a bound against the map's
    own length; the bound is skipped for Halo PC maps.
  - *The 31 MB tag space is kept for the run* rather than freed and
    re-allocated per map (a long session's heap couldn't give it back
    contiguous). The map checker reads the map into it while no PC map is
    loaded, saving its own 23 MB image.
  - BSP vertex data must be word-aligned (the checker refuses it
    otherwise), and non-finite positions and texture coordinates are
    zeroed in conversion. HUD placements not on a word are left as they
    are. `ui.map` tag addresses must be word-aligned.
  - *Network loop (third review):* each socket's port is remembered in
    `xnet.c` (`getsockname` was a socket-service call on every datagram
    sent); the link status the menus ask every frame is cached for 1 s;
    `posix_3ds_poll` sleeps a quarter of the wait left (4–16 ms) instead
    of 4 ms always (the internet-play thread alone made 250 polls a
    second); the Online Games rows are rebuilt when the lobby changes or
    every 250 ms (`p2p_lobby_generation`), not every frame; the tunnel
    and stand-in receive batches are capped at 32 and 16 a pass (the pass
    holds the lock the game thread's sends take); the internet-play
    tables are sized for the 3DS (96 stand-ins, 16 listeners, 64 streams,
    128 listed games); a join's connect waits 4 s, not 10, on the game
    thread. Not done: the password key derivation (Argon2id, 4 MiB, three
    passes, wire-fixed) still runs on the game thread when hosting or
    joining a password game — a stall of a second or more on the console.
  - Compiler warnings in the fork's own code: a split-screen test path's
    widget name had unescaped backslashes (the string was wrong), and
    int/pointer conversions in `ce_bsp.c`/`ce_models.c` are explicit.
- **Console test 2026-10-21 (evening): the server browser listed nothing,
  and leaving from the HOME menu crashed.** Both from the logs and dump:
  - *The list:* the game list is fetched over HTTPS, and every attempt
    logged "could not connect". The console's socket service reports a
    socket writable while its non-blocking connect is still under way,
    with the service's own "in progress" code (-26, negative, not
    converted by libctru) in SO_ERROR; the emulator reports 0. The port
    took that for a failed connect, for the HTTPS client and for the
    game's own TCP connects (`posix_socket_select`'s check, which also
    produced the "socket error errno -26" line). `posix_3ds_connect_state`
    converts the code with libctru's `_net_convert_error` and says
    connected / connecting / failed; the HTTPS connect and the select wait
    on "connecting" until it resolves or the time is up.
  - *The crash:* a data abort in the C library's `close` (reading a null
    device table entry): at the game's end `socExit` had removed the
    socket device while the internet-play thread was still running, and
    its `broker_close` then closed a socket. `close` is now wrapped
    (`port/n3ds/build.py`, `engine_soc_serial.c`): sockets are known by
    descriptor, and closing one after the service closed is no call.
- **Console test 2026-10-21, late: multiplayer works; the server browser
  opened a second time crashed** (`halo-source-fatal.log`:
  `engine_hardware_bitmaps.c:142`). The font atlas is re-uploaded for every
  glyph added, each upload retiring the previous texture until a GPU
  barrier reclaims it, and a barrier can't run inside a frame. Opening the
  browser again adds the whole screen's glyphs in one frame: 63 retired
  copies of 64 KB hit the 4 MB cap, and the assert stopped the game. Now,
  when no copy can be retired, the atlas is updated in place
  (`n3ds_gpu_texture_update`), as the Xbox's single atlas was; a cell
  replaced in that same frame may show its newer glyph in an earlier line
  for that frame. Reproduced and verified in Azahar (the same 63 retired).
- **Network version 38 (2026-10-23).** The fork followed OpenCE to 38
  (fork 8bc228b04); the 3DS followed with `fork_update.py` from 8ab6b1324:
  245 files merged, 48 added, 24 conflicted files resolved keeping both
  sides (`halo_port_capacity.h` rebuilt from the fork's with the 3DS's
  sizes; `platform.h` the fork's with the 3DS thread wrappers). Hosts
  announce 38 and join 11-39; Delta wire `chupa-32a`, legacy newest 38.
  `HALO_MAP_LIST` is defined beside `HALO_CUSTOM_EDITION`, as the fork's
  builds do. New units: OpenCE gametypes, infection, gun game, map packs,
  the v35 game settings layout, shield effects, the Delta profile and chat
  bridge, clan emblems, controller layouts, and `map_torrents.c` for the
  approved map list only (its BitTorrent fetches stay off on the 3DS).
  The desktop-only screens are stubbed; frame interpolation is off.
  - *Approved maps.* Map sharing now downloads only maps on the approved
    list, `map_torrents.txt` (the fork's `port/assets/network`, copied to
    `port/n3ds/assets/`), which must be in `halo-source/` on the SD card.
    Without it a download is refused with a message naming the file.
  - *Tested in Azahar:* the 3DS joined the fork's v38 Windows dedicated
    server (built from 8bc228b04 in `build/opence-v32`, `--without-ffmpeg`;
    scratchpad `server38.sh`, 3DS on 127.0.0.3) and played: "ChupathingyCE
    0.8.0f-dev, network version 38", capabilities agreed. The Online Games
    list shows the public v38 hosts.
  - *Azahar's socket service* stopped on a `getsockname` of a socket not
    yet bound (new in the v38 join). The socket wrappers
    (`engine_soc_serial.c`) now track which sockets are bound and connected
    and answer such calls EINVAL / ENOTCONN, as the console does; an
    address that isn't IPv4 is refused EAFNOSUPPORT.
- **Performance (2026-10-23).** From the console's logs of a joined public
  game (about 28 players): simulation 35-50 ms a frame (all of it
  `objects_update`), input 7-17 ms, and render hitches of 50-600 ms.
  - *Hitches:* the first time a texture is drawn it was read from the
    map's `.ntx` on the SD card. A map's archive (22-26 MB for multiplayer
    maps) is now read whole in the background as the map loads, into the
    Halo PC maps' space when that exists (kept for the run, free while an
    Xbox map is loaded, so nothing new fragments the heap), else into its
    own buffer when the heap keeps 20 MB besides. Blood Gulch's is in memory
    about 5 s after loading starts; the menus' (ui) is not read. A Halo PC
    map load takes the space back first. A first try with a buffer of its
    own fragmented the heap: extinction's model data then failed.
  - *Simulation:* the object sampler's hooks had been lost in a merge and
    are back (OBJECT TICK SAMPLE / OBJECT TYPE SAMPLE, plus postprocess),
    with PROJECTILE STAGES. In Azahar a local game with 15 bots
    (`config.bots.toml`) costs 18-21 ms of objects a tick against 2.7 ms
    alone: projectiles (0.2-0.5 ms each), living and dead bipeds, weapons
    being fired, bone matrices (3.5 ms) and attached lights (3 ms).
  - *Input:* SOC CALLS logs the socket calls by function every 10 s (the
    game thread's apart) and SOC RECEIVERS the receive callers. A joined
    game makes about 10 socket-service calls a frame on the game thread,
    two thirds of the receives finding nothing waiting.
  - *Next:* the console's counts from a full game (these lines in its
    log) decide between batching the receives behind one poll a frame and
    trimming the object update (projectile collision, lights, weapons).
- **Self-updater (2026-10-24, `port/n3ds/src/engine_update.c`).** A
  release's build (`build-docker.sh --release`; its version is
  `port/n3ds/VERSION`, `--version` overrides it) asks GitHub's API for the
  latest release of FractumSeraph/Fractum-Halo-3DS on a thread of its own
  once the console has an address. When the tag (`fractum-vX.Y.Z`) is newer,
  the main menu asks with the keyboard applet between frames (Update / Not
  now / Never, which writes `update.auto` false). Update downloads
  `Halo3DS.3dsx` (to the path in argv[0]) and `map_torrents.txt` as
  `<name>.new`, checks size and SHA-256 against the API's per-file `digest`
  (GitHub's own, computed on upload), then swaps each in (`.old` kept until
  the rename succeeds). No signing key: SHA-256 over HTTPS, as the user
  chose. GitHub's API is under Sectigo's E46 root (cross-signed by
  USERTrust ECC); its roots and USERTrust RSA/Sectigo R46 are added to
  `engine_tls_roots.h` (release files are Let's Encrypt's, already there).
  `debug.update_answer` answers for tests (`yes`, `no`, `never`,
  `yes-bad-hash`). Tested in Azahar with a release build numbered 1.0.9:
  it found 1.1.0, downloaded and installed it (the SD card's files then
  hash as the release's), and a wrong expected hash was refused leaving
  the installed files untouched; the prompt opened and Update worked.
  *Releasing:* bump `port/n3ds/VERSION`, build with `--release`, publish
  the tag `fractum-vX.Y.Z` with `Halo3DS.3dsx` and `map_torrents.txt`.
- **Fork 552a61ae7 (2026-10-25).** The last fork commit taken is now
  552a61ae7 (fork main): ChupathingyCE 0.8.0h (Delta wire `chupa-38a`, the
  legacy table's layouts, OpenCE 37/38 its way), a client keeping its
  inventory until the unit and weapons it names have come, the approved-map
  refusal in the error dialog's room, and the fork's performance changes
  (dynamic light limits, the sound mixer and game thread no longer waiting
  on each other, a host's stream read 32 KB at a time, release builds
  skipping the LRU caches' walk). `fork_update.py` now merges with LF line
  endings on all three sides: 28 files merged, one conflict
  (`map_torrents.c`, the 3DS's missing-list message kept beside the fork's
  new refusal). `HALO_CONSOLE_LIGHT_LIMITS` is on for the 3DS
  (`halo_console_features.h`), as on the fork's consoles: the 8 largest
  dynamic lights light the level, the 2 largest add their specular. Tested
  in Azahar: joined the fork's server built from 552a61ae7 ("ChupathingyCE
  0.8.0h-dev ... network version 38"), Chaos Gulch, 15 bots.
- **Fork 4a752e42f, network version 39 (2026-10-26).** The last fork commit
  taken is now 4a752e42f; fork main is at cfa6c9790, whose two commits
  after it touch only its README and a CI workflow. The merge brings
  network version 39 (joining 11 to 40), OpenCE's own dedicated server
  (`halo --server`: `server_mode.c`, `server_director.c` and the rest) and
  the server browser's master servers (`master_list.c`, which asks
  `https://master.opencommunityedition.com/v1/servers` every 30 s while
  Online Games is open and checks the list's Ed25519 signature).
  `fork_update.py`: 55 files merged, 4 conflicts resolved keeping both
  sides, 28 added; `port/third_party/jsmn` (the JSON parser) is now in its
  scopes. The 3DS is never a server: `server_mode.h` sets
  `HALO_SERVER_BUILD` 0 and `main.c`'s `main_server_mode()` is 0, so the
  server units compile to the fork's own stand-ins; no server code is
  changed. Three 3DS additions:
  - `update_https_request` (`posix_browser.c`, `HALO_N3DS`), the request
    `master_list.c` makes, on the browser's own TLS connections (body cut to
    256 KB).
  - *The clock.* The 3DS keeps the local time its owner set and no time
    zone, so `time()` is hours from UTC, and the master's list (dated within
    5 minutes) was refused in Azahar ("dated 17989 seconds ahead"), as the
    brokers' signed listings (within an hour) would be on a console set to
    a time zone an hour or more from UTC. Each HTTPS answer's `Date` header now sets the clock's offset
    from UTC (`posix_browser_utc_time`). `p2p_lobby.c` and `master_list.c`
    use it on the 3DS for their listings' times.
  - Stand-ins for `menu_advertised_game_from_peer` (only `halo --probe`
    asks) and `posix_server_private_file`. The 3DS sends no mod list.
  Tested in Azahar: Online Games lists the master's 18 servers beside the
  community list, OpenCE's dedicated servers among them ("OpenCE (legacy,
  network 39)"). The first ask, made before the console's network was up,
  failed to connect; the next round 30 s later answered.
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

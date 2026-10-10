# Developer notes

Build instructions, testing and maintenance notes for contributors, including
coding agents. Player instructions are in the [main README](../README.md).
Commands below run from the repository root unless stated otherwise.

- [Building the game](#building-the-game)
- [Building the setup tools](#building-the-setup-tools)
- [Setup formats and recovery](#setup-formats-and-recovery)
- [Engine and platform boundary](#engine-and-platform-boundary)
- [Rendering rules](#rendering-rules)
- [Stability and timing rules](#stability-and-timing-rules)
- [Circle Pad Pro input](#circle-pad-pro-input)
- [Local wireless multiplayer](#local-wireless-multiplayer)
- [Verification](#verification)
- [Setup and repository maintenance](#setup-and-repository-maintenance)

## Building the game

The build uses Python 3.10+, Clang, devkitARM, libctru, citro3d, Picasso and
3dsxtool, plus locally supplied August 2001 XDK headers. SDK files and game
data are not included. Players do not need these dependencies.

The build has been checked on Windows. Use native Windows toolchain paths;
a DEVKITARM value from WSL is not a Windows toolchain. Validated dependency
revisions are libctru `36fe1ada5b7ebe53ba4decda36d764a55f8fefb6` and citro3d
`9f21cf7b380ce6f9e01a0420f19f0763e5443ca7`.

Replace the dependency paths with yours, quoting any that contain spaces:

```powershell
python port/n3ds/build.py `
  --devkitarm C:/devkitPro/devkitARM `
  --clang C:/tools/llvm/bin/clang.exe `
  --libctru C:/deps/libctru/libctru `
  --citro3d C:/deps/citro3d `
  --xdk C:/private/xdk/include `
  --picasso C:/tools/picasso.exe `
  --packer C:/tools/3dsxtool.exe
```

Output is `build/n3ds/Halo3DS.3dsx`, the playable main-menu build for both
Original and New models. `--help` lists environment overrides and output
options.

`port/n3ds/sources.json` lists the native replacements and required original
engine units. `tools/compat.py` and `tools/shaders.py` in that directory prepare
the SDK overlay/linkage adapters and assemble PICA shaders. Generated headers,
response files, logs, ELF/map and the build/hash manifest stay in the output
directory. Retain the matching ELF/map for crash diagnosis.

Incremental builds track source/header content. Choose a new `--output` directory
for clean verification. Player releases must use the default options, without
diagnostics, a frame limit or a test heap.

### Diagnostic builds

Add `--test-frames 240 --diagnostics --output build/n3ds-diagnostics` to the
dependency command above for a bounded menu run with renderer checks. Success
writes `RESULT: PASS` to the game log. This build exits automatically.

`--original-memory` constrains the heap for Original-model emulator tests.
Also select an Original model in the emulator; neither setting replaces a
test on actual Original 3DS/2DS hardware.

## Building the setup tools

Both setup interfaces prepare 3DS game data using the shared converter in
`port/n3ds/setup/Core/`.

### Visual Studio

Open **File > Open > Project/Solution** and choose the solution for the edition
you want to build:

| Edition | Solution | Configuration | Startup project |
| --- | --- | --- | --- |
| Windows 10/11 | [Halo3DS.Setup.WinUI.sln](../port/n3ds/setup/Halo3DS.Setup.WinUI.sln) | Debug / x64 | `Halo3DS.Setup` |
| Windows 7 | [Halo3DS.Setup.Windows7.sln](../port/n3ds/setup/Halo3DS.Setup.Windows7.sln) | Debug / x86 | `Halo3DS.Setup.Windows7` |

For WinUI, install Visual Studio 2026 (18.x or newer) with the **WinUI application
development** workload, .NET 10 and Windows SDK tools. See Microsoft's
[WinUI tools guide](https://learn.microsoft.com/en-us/windows/apps/get-started/start-here)
and [.NET/Visual Studio support matrix](https://learn.microsoft.com/en-us/dotnet/core/porting/versioning-sdk-msbuild-vs).
For the Windows 7 solution, install the **.NET desktop development** workload,
the .NET SDK, .NET 3.5 reference assemblies and Python 3 on your development PC.

1. In `port/n3ds/setup/`, copy [Setup.local.props.example](../port/n3ds/setup/Setup.local.props.example)
   to `Setup.local.props`. Set `GameBuild` to the full path of your
   `Halo3DS.3dsx`, and `PythonExecutable` to `python` or your Python executable's
   full path. This local settings file is excluded from Git and source packages.
2. In Solution Explorer, right-click the startup project listed above and choose
   **Set as Startup Project**. Select the configuration and platform in the toolbar.
3. For WinUI, allow NuGet packages to restore. Choose **Build > Build Solution**
   (**Ctrl + Shift + B**) to build either edition.
4. For WinUI, press **F5** to run, then select your game executable under
   **Additional options**. A normal IDE build does not bundle the game.
   For Windows 7, run `build/setup-win7/Debug/Halo3DS Setup Windows 7.exe`
   from the repository root; this build already embeds your selected game.

Switch **Debug** to **Release** for a release build. The Windows 7 EXE will be
under `build/setup-win7/Release/`. Its solution calls the same build script as
command-line packaging. For a single-file WinUI release, use the
[packaging command below](#windows-1011-release); the bundle launcher is excluded
from normal solution builds because it needs a packaged ZIP.

### Windows 10/11 release

Build on Windows with .NET SDK 10 and Windows SDK build tools:

```powershell
dotnet run --project port/n3ds/setup/Tests
dotnet publish port/n3ds/setup/App -c Release -r win-x64 -p:Platform=x64 -o build/setup
```

`dotnet publish App` produces a folder of files. For the distributable single
EXE, run the packaging script instead, using a new output directory:

```powershell
./port/n3ds/setup/tools/package.ps1 `
  -Output C:/build/halo-setup `
  -GameBuild C:/build/Halo3DS.3dsx `
  -VCRuntimeDirectory 'C:/path/to/VC/Redist/MSVC/version/x64/Microsoft.VC143.CRT'
```

### Windows 7 release

Build dependencies are the .NET SDK's Roslyn compiler, .NET 3.5 reference
assemblies and Python 3. The finished x86 EXE runs on 32-bit or 64-bit Windows 7
with its .NET Framework 3.5.1 component enabled. It does not ship modern .NET,
WinUI or Visual C++ runtimes.

```powershell
./port/n3ds/setup/Windows7/build.ps1 `
  -GameBuild C:/build/Halo3DS.3dsx `
  -Output C:/build/halo-setup-win7 `
  -Python python
```

Output must be outside the setup source tree. Distribute only
`Halo3DS Setup Windows 7.exe`; `LegacyTests.exe` and `Source.zip` are developer
outputs. The app's **Source / licenses** button can export its embedded source ZIP.

`Windows7/adapt-core.py` generates `Windows7/Core/` from the shared core.
Edit conversion logic in `Core/`, not generated copies. `Compatibility.cs`
provides bounded reads, SHA-256, JSON, raw DEFLATE behind a validated zlib header,
and same-volume replacement using `MoveFileExW`. The compiler uses `/nostdlib`,
only .NET 2.0/3.5 references, `/codepage:65001`, a Windows 6.0 PE subsystem
minimum and a DPI-aware manifest.

### Setup source and release checks

The setup source is organized as follows:

| Directory under `port/n3ds/setup/` | Purpose |
| --- | --- |
| `App/`, `Windows7/` | WinUI and Windows Forms interfaces |
| `Core/` | XDVDFS reads, map conversion, HUD/loading extraction, validation and installation |
| `Tests/` | Safety checks and local-data integration tests |
| `Resources/` | Supported-file hashes and format descriptions |
| `Launcher/` | Self-contained WinUI bundle launcher |
| `tools/` | Packaging and developer-only metadata generation |

`tools/source-files.ps1` defines the source files shared by both packages.
Packaged sources preserve repository-relative paths and include this document
and the main README. They contain the setup utility, not the complete game
source; use the repository to build the game. Python is never needed at runtime.

Verify the game hash extracted from both final EXEs. Check that source archives
contain the converter, adapters, solutions, scripts, schemas and notices, but no
maps, Xbox executables, generated relocation files, SDK headers or DSP firmware.
The Invader schemas are GPL-3.0-only, revision
`a497b7457640dc2ee99fe8ad480d669e57347ef1`; preserve the
[component notices](../port/n3ds/setup/THIRD-PARTY.md) and corresponding setup source.

Run the asset-free modern tests above and these legacy modes. The last argument
must name a fresh test directory:

```text
LegacyTests.exe safety OUTPUT
LegacyTests.exe package "Halo3DS Setup Windows 7.exe" Halo3DS.3dsx OUTPUT
LegacyTests.exe audit EXPANDED_MAPS OUTPUT
LegacyTests.exe exercise XISO EXPANDED_MAPS Halo3DS.3dsx OUTPUT
```

The `package` check inspects the compiled interface for garbled text and verifies
its embedded game. Integration checks cover compressed/expanded inputs, all 24
maps, cancellation, repair, repeat installs, launcher updates and save preservation.
These have been exercised on the development PC; the legacy app has run under
CLR `2.0.50727.9179` in a 32-bit process. An actual Windows 7 OS/VM test and a
full Linux/Wine runtime test have not been performed.

## Setup formats and recovery

The accepted Xbox release is English North American retail build
`01.10.12.2276`. Setup validates exact hashes, including compressed and unchanged
expanded map versions. Other revisions/regions, modified maps and PC/MCC assets
are rejected. A complete dump has 24 maps: `ui`, 10 campaign and 13 multiplayer.
A partial set needs `ui.map` and at least one playable map.

- XISO input accepts a single `.iso` or `.xiso`, including recognized game
  partition offsets. Extract ZIP/7z files and join split images first. Images
  are read directly, never mounted. The format was checked against
  [XboxDev's extract-xiso](https://github.com/XboxDev/extract-xiso/blob/master/extract-xiso.c);
  that extractor is not linked or executed.
- Folder input searches the maps folder or its parent game directory for the
  matching `default.xbe`. It supplies the HUD strings and optional loading art;
  it is never executed or installed. Maps-only input can use an existing
  `ntsc2276-hud-strings.bin`, including a verified copy on the destination.
- Setup creates `halo-source/ntsc2276-loading.bin` from a supported XBE when
  available. Existing artwork alongside maps is also accepted. Without it,
  text-only loading still works. A normal setup run adds artwork while skipping
  correct maps; a launcher-only update does not extract assets.
- A normal output folder can be used instead of a card. Copy its `3ds` and
  `halo-source` folders to the card root afterward. Under Wine, selected paths
  and their ancestors may be links/mapped drives; links inside the destination
  are rejected.
- The DSP check only tests for a nonempty `3ds/dspfirm.cdc`. It does not verify
  firmware authenticity or download firmware. Setup provides Rosalina directions.

Rerunning full setup verifies and skips correct maps, repairs damaged groups,
and keeps `halo-source/state`, other homebrew and unrelated files. A legacy
`HaloSourceEngine.3dsx` is backed up under `halo-source/launcher-backups` so it
doesn't remain as a duplicate Homebrew Launcher entry.

Cancellation keeps completed maps and removes the current temporary map.
After a disconnect or power loss, rerun setup before launching. Exceptional
replacement failures can leave `.halo-setup-*` recovery folders; these are not
silently deleted. Reads and conversion are bounded; source, expanded and
relocation hashes are checked and destination files are read back before commit.
Setup logs remain in `%LOCALAPPDATA%\Halo3DS Setup\Logs`; nothing is uploaded.

## Engine and platform boundary

`source/` retains the upstream engine's subsystem layout: AI, physics, objects,
game state, scripts, rendering and tag definitions. Keeping this organization
makes upstream comparisons and crash diagnosis easier. The native implementations
live under `port/n3ds/`; they are not a separate reimplementation of gameplay.

The Xbox data layout remains 32-bit with 16-bit wide characters. Engine units are
compiled with Clang's MSVC extensions; native libctru/citro3d units use devkitARM.
The two compiler groups are explicit in `sources.json`. Do not casually change
signed-char, short-wchar, common-symbol, floating-point or alignment options.

`compat/` contains the small CRT and weak-global definitions still needed from
upstream. `tools/compat.py` copies locally supplied XDK declarations into the
build directory, replaces three x86-only shift helpers with C, and generates
the forward declarations/weak-inline annotations required by the original C
linkage rules. It does not ship SDK headers or invoke Xbox code.

## Where to make changes

| Area | Main owners |
| --- | --- |
| Menu and game lifecycle | `engine_frontend_boot.c`, `engine_main.c`, `engine_mission_render.c` |
| GPU submission and materials | `engine_gpu_renderer.c`, `engine_gpu_chicago.c`, `engine_gpu_widgets.c` |
| Models and scenery | `engine_models.c` and its model/scenery helpers |
| Textures and map loading | `engine_textures.c`, `engine_cache.c`, `cache_reader.c` |
| HUD, touch aim and controller settings | `engine_hud.c`, `engine_input.c`, `engine_controls.c` |
| Audio and worker jobs | `engine_audio_platform.c`, `engine_parallel_platform.c` |
| Persistent game state | `game_state_n3ds.c`, `engine_persistent_state.c`, checkpoint helpers |
| Asset preparation | `setup/Core/`; Windows 7 adaptations are generated from it |

The `.inl` files are deliberate pieces of an owning translation unit, often
sharing private renderer state. They are not independent libraries. Consolidate
only when responsibilities match; combining unrelated material paths into a
larger file does not simplify their state and memory contracts.

## Rendering rules

- PICA200 arithmetic has less precision than CPU floats. Model palettes, CPU
  model uploads, glow sprites and particle transforms use camera-relative
  coordinates. Keep shared animation/pose data in world space and rebase upload
  copies, using the current eye's camera. UI vertices remain in screen space.
- Depth and color passes must use the same pose and coordinate system. Lighting,
  projective texture coordinates and fog must be calculated in their matching
  coordinate system before rebasing. Repeated draws must not rebase source data
  in place.
- GPU commands retain vertex/index addresses until completion. Do not wrap,
  free or rewrite queued storage. Texture/vertex changes must respect cache
  flushes and the existing frame barriers.
- Restore material/sampler state and invalidate the submission caches when a
  different path changes their assumptions. Native combiners and lighting can
  avoid passes, but hardware limits still constrain each material.
- Original models have smaller memory and rendering budgets. Use model checks;
  do not lower New-model quality to fix an Original-model allocation failure.

## Stability and timing rules

Compressed animation streams can contain unaligned floats. Keep byte-safe loads
and aligned local results; casting packed data to float/vector pointers can cause
ARM11 faults. Use typed, aligned collision scratch structures for the same reason.

Gameplay state, scripts and rendering have shared ownership. Worker jobs operate
on disjoint data with explicit lifetime fences; do not move arbitrary AI or object
mutations to another core. Cache keys must include pose/instance mutations and
must not survive invalidated map or tag storage.

Checkpoint work uses bounded buffers, validated data and staged replacement.
Preserve the existing cancellation, shutdown, Save and Quit, and map-transition
ordering. A changed build identity may invalidate incompatible checkpoints;
preserving a file is not proof it is safe to resume in a different engine layout.

The dynamic-resolution policy must account for sustained load and recovery
delays. Do not react to every isolated slow frame: frequent resolution changes
can create visible oscillation and stalls. Keep UI resolution and world rendering
quality decisions separate.

New models retain the full-resolution baseline and reduce world resolution
only under GPU wait pressure. Original models start the world at 320x192 and
can trial 256x152 or 200x120 when GPU waits or sustained CPU rendering cost
justify it. Lower projected size also selects cheaper authored model LODs;
resolution alone cannot fix slow AI. Trials must demonstrate a timing benefit,
and recovery needs sustained headroom. Original world targets use RGB565 to
reduce VRAM use and color bandwidth, with format-correct framebuffer clears.
Menus and HUD remain full-resolution RGBA8 on both families. Map conversion
does not select these budgets: the same executable detects the console model.
Original gameplay also omits model reflection/detail textures, uses stricter
authored mesh LODs, thins disconnected foliage islands and limits decorative
particles. Base textures, cutouts, team colors, emission and native lighting
remain. These reductions do not change collision meshes or AI update rates.

## Circle Pad Pro input

The optional accessory driver is `engine_circle_pad_pro.c`. Enable it in
Controller Settings; it defaults off so normal play does not open `ir:USER` or
allocate a worker. New models retain the built-in IRRST C-stick as a fallback;
fresh CPP data takes priority when the option is enabled. CPP axes
use the same dead zone and Right-stick aim sensitivity as the C-stick; button
mapping and touch aiming are unchanged. The software option is available on
all models, but physical cradle fit and IR alignment on New models and the
Original 2DS are not assured. Nintendo only supports Original 3DS/XL cradles.

The worker owns IR IPC, one 4 KiB shared page and an 8 KiB stack. The game thread
only copies a locked sample. Calibration reads are checked for CRC, response
length/offset and finite scales; frames are decoded without unaligned loads.
Receive metadata and ring boundaries are validated before access. A missing
accessory retries after a one-second wait; unavailable IR does not fail boot.
Samples older than 200 ms are ignored. HOME/sleep clears input, invalidates the
session and reconnects after restore. Shutdown signals and joins the worker
before freeing its resources. No calibration-write commands are sent.

Controller settings version 2 adds the opt-in field and reads version 1 without
losing existing sensitivity/layout preferences. `circle_pad_pro_tests.c` runs
on a host compiler and covers calibration, CRC, packed axes and corrupt/wrapped
packets. Controller settings tests also cover version 1 migration and version 2
validation. A bounded Azahar test covers menu boot, emulated CPP calibration,
disable/re-enable and worker shutdown on both Original and New models. Hardware
calibration, reconnection, sleep/HOME and simultaneous UDS play still require an
actual accessory; emulator checks cannot validate the physical IR connection.

Protocol references: [Circle Pad Pro](https://3dbrew.org/wiki/Circle_Pad_Pro),
[IR shared memory](https://3dbrew.org/wiki/IRUSER_Shared_Memory) and
[initialization](https://3dbrew.org/wiki/IRUSER:InitializeIrnopShared).
The calibration field layout was cross-checked against the
[Red Viper driver](https://github.com/skyfloogle/red-viper/blob/master/source/3ds/cpp.c).
The driver here is a separate implementation with a bounded parser and explicit
thread/resource ownership.

## Local wireless multiplayer

System Link uses libctru UDS for nearby 3DS-to-3DS communication. It does not
use internet sockets or connect to Xbox consoles. The original Halo menus,
host/client managers, game variants, action updates and synchronization checks
remain in use. Native frame servicing runs the original network callbacks
before simulation and sends local updates afterward. Solo play does not
initialize the radio or allocate its queues.

The adapter has three parts:

- `engine_network.c` implements the original transport interface with
  session-local node addresses and nonblocking endpoints.
- `engine_wireless_protocol.c` implements bounded datagram queues and reliable
  byte streams, including fragmentation, acknowledgments, retries and timeouts.
  The explicit little-endian wire header is versioned and validated before use.
- `engine_wireless_platform.c` owns UDS discovery, connections and the radio
  worker. Game objects remain on the main thread; endpoint state is locked.
  The worker continues servicing packets while a map loads and has a bounded
  receive batch so it cannot monopolize the CPU.

The radio advertises an application ID, protocol marker and build fingerprint.
Only matching builds connect. Halo map settings carry an identity derived from
the validated prepared-map relocation header; clients check it before loading.
Normal map loading still performs the full relocation validation. These are
compatibility checks, not cryptographic authentication. The UDS passphrase is
a public application namespace, not a user password.

The initial limit is four consoles, one local player each. Discovery attaches
to the first compatible non-full radio host; the original game browser then
lists that host's session. It does not merge listings from multiple radio hosts.
Campaign co-op, online play and joining a match already in progress have not
been validated. Sleep/HOME suspension ends the local connection. Client and
server loading allowances are longer than Xbox defaults to accommodate slow
SD cards, while transport heartbeats still detect a silent peer.

Do not restore the original per-packet traffic spreadsheets: their synchronous
flushes cause SD I/O during gameplay. Session events use the existing engine log.

The asset-free protocol test can be built with a host C11 compiler, for example
from a shell configured for Clang and its system C runtime:

```sh
clang -std=c11 -Wall -Wextra -Werror -Iport/n3ds/include \
  port/n3ds/src/engine_wireless_protocol.c \
  port/n3ds/tests/src/wireless_protocol_tests.c -o build/wireless-tests
```

Run the resulting executable. Add `-fsanitize=address` when supported. Tests
cover packet loss, duplicate/reordered delivery, fragmentation, full/wrapped
queues, malformed packets, discovery before connection, loopback, reconnect,
peer loss and four-console stream isolation.

Integration testing uses separate emulator SD folders and a private local
Azahar room to carry emulated UDS traffic. Test-only controller injection and
simulation-state sampling must not be linked into the distributed executable.
Two New-model emulators have completed discovery, lobby join and Blood Gulch
loading, movement and charged plasma fire, with matching random-state samples
at 127 shared simulation ticks. Host disappearance and client menu exit were
also checked. A mixed New/Original-model emulator match loaded Blood Gulch,
rendered the other player and exercised movement and firing, with matching
random-state samples at 207 shared ticks. The Original-model client used
24 MiB ordinary and 32 MiB linear heap limits. These samples are not full
game-state checksums. That client also returned to the menu, released its
wireless buffers and loaded Pillar of Autumn's opening cinematic offline.
This is not proof of radio reliability, four-console gameplay,
all game variants, or performance on physical consoles. Keep the wireless build
separate from the stable installers until hardware testing is complete.

## Verification

### Multiplayer post-game lifecycle

The native frontend must call `game_engine_update_non_deterministic` once per
frame while a game is loaded, even when simulation ticks have stopped. This
advances the carnage report's five-second delay and handles the original
buttons: A/Start returns the host to match setup, B/Back leaves the session.
Leaving this call out strands the host on the report after the match ends.
Keep it outside the tick loop; it must not depend on incoming player actions.
Regression checks should cover the last guest leaving, a normal match end,
returning to match setup, and leaving for the main menu on both model families.

### Campaign co-op development

Campaign co-op is under development and is not part of the stable installers.
The target is two consoles, with one player on each. Multiplayer > Co-op opens
the wireless profile/browser flow with installed campaign maps and their
original preview images; the following selection chooses difficulty. System
Link retains competitive maps only. Both peers must use the same build and
map data.

Menu intent is separate from the selected map: it must survive profile and
lobby navigation, even before a host exists. Co-op and competitive rooms use
different UDS sub-IDs, so discovery cannot attach to a room from the other
menu. The radio worker uses a session-fixed sub-ID. Campaign thumbnails reuse
the existing `sp_levels` bitmap at render time without modifying map files.
Lobby machine names default to the CFG-reported model, including Original
3DS/XL and 2DS, with a neutral fallback when the model query fails.
The menu update was checked with an Original-model emulator hosting and a New
model joining: campaign previews matched the installed missions, the lobby
identified the host as Nintendo 3DS XL, and 169 shared simulation-state samples
matched through tick 1,232 after entering Assault on the Control Room. System
Link separately showed only the two installed competitive maps and their
previews. The Original peer also switched from Co-op to competitive discovery
without restarting. Physical model naming and wireless behavior still need
the corresponding hardware check.

The design reference is [OpenCE's networking notes](https://github.com/OpenCommunityEdition/OpenCE/blob/07302a866f8b36c162203fe58dcf632d4708d65c/port/linux/NETCODE.md)
(the project formerly called halo-ce-universal). OpenCE replaces Xbox lockstep
with host-authoritative AI, scripting and object replication. This prototype
retains the existing lockstep transport: both peers simulate campaign logic.
Camera timing and campaign services therefore follow shared simulation ticks,
and distance-based simulation shortcuts that depend on the local view are
disabled in co-op. Rendering optimizations remain local.

Audio-driven mouth animation is omitted in co-op. Its local playback timing
otherwise changes simulation bones and weapon attachment markers, allowing
combat to diverge even when player inputs match. Speech still plays, and solo
lip animation is unchanged. Fallen-body settling also uses a common tick
schedule on Original and New models.

Checkpoint snapshots stay local to each running session and do not overwrite
solo disk saves. Network update numbers must remain monotonic across a campaign
clock rewind. Skip and revert requests travel in reserved player-action bits,
which are stripped before applying unit controls. A pause menu does not pause
the other console. Restart returns the pair to the lobby; mission completion
selects the next campaign map there. These lifecycle paths require integration
validation before promoting a co-op build to stable.

The client must wait for a validated settings packet before reporting map
readiness. The pregame loop can run between the join acknowledgement and that
packet; using the menu's previous map at that point could enter the legacy
damaged-media loop.

Paired-emulator checks exercised PoA movement, shared cutscene skipping,
manual checkpoint restore and restarting through the lobby. A Silent
Cartographer combat run matched random-state and object-state samples through
5,700 network ticks, individual respawn, all-player death/checkpoint recovery,
and a forced mission-completion handoff to Assault on the Control Room.
An Original/New configuration pair, with 24 MiB ordinary and 32 MiB linear
heaps on the Original client, matched through another 3,300-tick run including
combat and all-player recovery. The detailed combat trace compared more than
540,000 object records, including weapon attachment bounds. Forced deaths and
completion exercise the real lifecycle functions but are not a campaign
playthrough. These tests do not establish physical-console performance.

A mixed-model Assault on the Control Room test crossed a loading zone from
BSP 3 to BSP 0, bringing the remote player across, with 691 matching state
samples through 3,330 network ticks and 265,478 matching detailed object records.
The probe used a collision-validated position inside the loading volume;
placing a player at the raw volume center was outside valid destination geometry
and correctly hit the original engine's missing-player assertion. Campaign maps
must open their campaign pause widget, since the competitive pause widget is
not included in their tag data.
The final mixed-model check opened that menu while both simulations continued,
matched 646 state samples through 1,980 ticks, and used the client's Save and
Quit entry to return both peers to the main menu without a fatal or sync error.
The original widget still says Save and Quit (and briefly shows its saving
message), but co-op exits without writing a persistent solo checkpoint.

Compare random-state and object-state samples at the same **network update
number**, not just campaign time: checkpoint reverts repeat campaign ticks.
Test independent movement, combat, doors, one-player respawn, all-player death,
manual checkpoint restore, BSP transitions, cutscene skips, mission completion,
leaving and mixed Original/New hardware. Idle cutscene synchronization alone
does not establish playable co-op. Never ship the input-injection fixture.

### Regression checklist

Test these paths after changes to memory, rendering, loading or controls:

1. Menu loading text/artwork, animation, text clarity, sound, profiles and settings.
2. Pillar of Autumn at a selected difficulty: opening, cryopod tutorial, touch
   aiming before the radar appears, and the Keyes cutscene.
3. Solo Blood Gulch: movement, aiming, pickup, firing, reload, sniper scope,
   flashlight/night vision, HUD and grenade selection.
4. Controller sensitivity/layout settings, Original-model L/R and touch input,
   and New-model C-stick and ZL/ZR input.
5. Stereo on/off on a 3D-capable model, pause/resume, Save and Quit, menu return,
   checkpoint resume across launches, BSP changes and HOME > Close.
6. Sustained campaign progression, dense scenery/AI/effects, The Maw timer,
   cinematic overlays and the particular scene affected by the change.

Use both model families, keeping a known-good executable. Compare the same
location/action, stereo state and controller settings. Emulator results are
useful for regressions and matched-workload timings, but are not hardware FPS
measurements or proof of campaign stability. Precision tests model PICA input
rounding because an emulator may retain more precision and hide a hardware error.

Native checks live in `port/n3ds/tests/`; some use `.inl` includes to access their
owning C file's private state. Release linking discards unused test functions.
Use the [diagnostic build options](#diagnostic-builds) for bounded runs and
user-supplied game data for local integration tests. Player log locations and
reporting steps are in the [README](../README.md#reporting-problems). Record
the exact executable hash, whether the issue repeats, and a photo if no dump exists.

When changing native code, keep a known-good build, record the new executable
hash, retain its ELF/map privately, and test both model families. Build outputs,
SDK headers, maps, saves and tester logs belong outside Git.

## Setup and repository maintenance

Change conversion logic in `setup/Core/`, then rebuild the Windows 7 adaptation
with its build script. Verify that both final setup EXEs embed the intended
3DSX; source version numbers alone do not prove the bundled payload is current.
Use UTF-8 explicitly in maintenance scripts. Legacy UI punctuation uses plain
text, with a compiled-interface regression check for the previous encoding bug.

Before distributing source, run `python tools/check_repository_contents.py --working-tree`,
then run it again without that flag after staging. `--git`
selects a Git executable if needed. This checks known exclusions and common
secret formats in the current tree/index, not all possible secrets or licensing
requirements. Only the current working tree or index is scanned.

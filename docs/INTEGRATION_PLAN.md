# Halo3DS integration notes

PR [#3](https://github.com/Generalkidd/Halo3DS/pull/3) was merged into `dev`
on October 11, 2026 (merge commit `754ce9f`). It combines the PC-map and local-bot
contribution from #1 with the existing wireless multiplayer/co-op work.
`main` has not been updated. The merged game source is the same as the tested
`6fee7f8` build copied to the owner's SD card; this merge does not require
another executable update.

The sections below retain the original contribution's implementation and
validation history. References to its earlier branches or New-3DS-only scope
are historical, not the current release policy. The follow-up checklist at
the end is the current list of outstanding integration work.

## What players receive

The [unified installer guide](../port/n3ds/setup/Unified/README.md) is the
player-facing starting point. The Windows PowerShell installer takes the
player's own supported Xbox XISO and either their own PC ISO or extracted PC
`maps` folder. It checks both sources before installation, supports `-WhatIf`,
copies verified files to the SD card, and keeps a backup of the previous
launcher. The distributable contains the launcher, setup tools, source, and
license notices. It contains no game maps, disc images, XBE, HUD, audio,
firmware, or private SDK files.

The Xbox menu retains campaign and multiplayer. SELECT opens PC local
multiplayer. Its lower screen shows map and equipment previews, and the gear
page selects PC map weapons and vehicles. The campaign difficulty screen has
a saved switch for optional PC additions. These are runtime objects: Xbox map
files and their original placements are never changed.

## Where the changes live

| Area | Main files | Purpose |
| --- | --- | --- |
| PC cache loading | `port/n3ds/pc/cache_file_formats.c`, `engine_pc_runtime.c` | Validate and read owner supplied retail v7 maps. |
| Menus and previews | `port/n3ds/src/engine_frontend_boot.c`, `port/n3ds/preview/` | Choose a map and show its geometry and equipment. |
| Gameplay | `port/n3ds/src/engine_bots.c`, `source/game/`, `source/items/`, `source/units/` | Solo local matches, bots, and PC weapon and vehicle rules. |
| Campaign additions | `port/n3ds/src/engine_campaign_options.c`, `engine_campaign_extras.c`, `engine_campaign_overlay.c` | Save the switch, place optional pickups, and import the Rocket Warthog dependencies for b30. |
| Setup | `port/n3ds/setup/Unified/`, `port/n3ds/setup/PC/`, `port/n3ds/setup/Core/` | Validate sources, install a single launcher, and package source without game data. |

The [developer notes](DEVELOPMENT.md) contain build commands and test details;
the [modding guide](../port/n3ds/MODDING.md) points to code for common edits.
The repository's `AGENTS.md` records attribution requirements for the New 3DS
integration. [License scope and credits](../LICENSING.md) identifies Kevin
Boggs's original MIT-covered additions separately from the original CC0
project and the GPLv3 setup component. Existing third-party notices remain intact.

## Review and merge recommendation

1. Review the data boundary first: `tools/check_repository_contents.py`,
   the fixed file list in `Package-Halo3DS-Unified.ps1`, and an extracted package.
   Build outputs, toolchains, private maps, and emulator SD folders stay out of Git.
2. Review Xbox regression and menu navigation on original 3DS, New 3DS, and
   emulator. Keep the default Xbox release route intact while introducing the
   unified launcher as an experimental choice.
3. Review PC cache conversion and one local multiplayer map at a time. Exercise
   weapon and vehicle pickups, bot matches, return to lobby, saves, sound, and
   frame time under combat on physical hardware.
4. Review campaign additions separately. The Xbox caches already contain the
   hidden Flamethrower and Fuel Rod definitions used in c20, c40, and d20.
   Runtime placement waits for the player to approach and confirms the active
   BSP contains the point. The b30 Rocket Warthog instead loads 16 missing
   dependencies from the player's PC Blood Gulch cache; a missing or rejected
   source leaves the Xbox mission playable. Test mission progression,
   checkpoints, HUD, driving, and the switch's off state on hardware.
5. Rebuild from the reviewed tree and test the **same 3DSX hash** that would be
   distributed. Keep its ELF and linker map for crash diagnosis. Merge only
   after the author is satisfied with hardware behavior and distribution terms.

## Evidence and open checks

On 2026-10-10, the current diagnostic build completed two PC menu and gear
navigation replays and the c20, c40, and d20 campaign replays in Azahar 2126.2
without a fatal error. Its logs recorded all four optional weapon pickups in
valid mission clusters: one in c20, one in c40, and two in d20. An extracted
installer package passed a PC map `-WhatIf` check. An invalid Xbox source was
rejected before any destination write. The b30 Rocket Warthog appeared beside
an original Warthog in an earlier emulator run; emulator
replays entered the vehicle and fired its mounted gun through the normal input
path. A previous New 3DS XL build completed a six-bot Timberland match. Those
checks show that the paths work, but they do not establish complete campaign
playthroughs or a steady 30 FPS. The current launcher still needs physical
New 3DS testing for campaign progression, visual quality, effects, busy bot
combat, and save and restore behavior. Emulator results should be reported as
emulator results.

The PC map selector now keeps every BSP triangle in the 19 supported retail
multiplayer maps; the previous 2,048-triangle limit left visible gaps. Death
Island scales its oversized decorative water only for the preview and uses dry
terrain to set the initial zoom. The lower screen gives the map most of its
space, with mode, bots, difficulty, and texture preference on the top screen.
Gephyrophobia now frames the central bridge. Infinity's blue NAV preview
shows its connected playable component and omits the disconnected upper rim;
the 15-bot CTF emulator replay followed routes toward both flags and recorded
combat hits without a fatal error. Bot combat acquisition now requires an
unblocked game collision ray. A recently hit bot can turn toward its recorded
attacker while objective routes keep priority and hidden targets cannot be
fired on.
Emulator replays checked map selection, gear/back navigation, option changes,
Death Island, and the optional NAV overlay without a fatal error. A New 3DS XL
Death Island match with seven bots crashed in an earlier launcher while a
layered shader requested a texture at the 8 MiB cache limit. The current code
reuses its base texture for that layer when allocation fails. The user reported
no crash on Death Island with the newer launcher; the exact tank-firing path
has not yet been confirmed. A later isolated arsenal replay verified that
SELECT leaves the original top scoreboard visible. Holding the touch SCORE
button instead drew the expanded top roster with K/D and match medal counts,
and left the lower controls unobstructed. The flamethrower sprite UVs were
rotated to put the atlas's rounded ignition edge at the bottom of each sprite.

The 16-player limit also had a focused emulator stress pass. A diagnostic
build replayed the PC Local menu on Death Island, selected CTF, raised the bot
count to 15, and selected all PC vehicles. The engine registered all 15 bots
beside the local player, created both CTF flags, and reached frame 1,800
(1,040 gameplay ticks) without a fatal error. The gameplay capture showed the
world and HUD. The CTF variant enables teams and the local player plus bots
are assigned to alternating teams in source, yielding eight players per side.
The game reported 16 active bipeds and 46 vehicle objects during this run.
After gameplay tick 800, the emulator's sampled frame time had a 37.6 ms
median, above the 33.3 ms target for steady 30 FPS; one sample reached
56.5 ms. These are emulator timings, not New 3DS XL measurements. A longer
physical match with moving players, combat, vehicles, and flag captures is
still needed before calling full-roster performance or stability proven.

A second emulator pass started the same Death Island CTF roster with high PC
textures and ran past frame 2,400, or about 77 seconds of gameplay. It
confirmed 16 players, bot pathing, weapon pickups, hits, grenades, melee,
vehicle entries, and a player death and respawn, with no fatal error. Its
sampled frame time after tick 800 had a 30.6 ms median and a 65.2 ms 95th
percentile as combat intensified. These varied emulator timings reinforce the
need for a measured New 3DS XL run before promising steady frame rate. The
player-fire diagnostic originally missed the saved shoulder-button swap;
the fixture now follows that setting and requires an actual shot to pass. A
follow-up replay passed: the assault rifle's loaded rounds fell from 60 to
45, and the engine recorded a nonzero firing time without a fatal error.
A separate 15-bot CTF postgame replay ended the match, used the lower-screen
Return to Lobby control, and reopened the PC map menu without a fatal error.

For a reproducible review, run `python port/n3ds/tools/check_attribution.py`
and `python tools/check_repository_contents.py --working-tree`,
build with the command in [developer notes](DEVELOPMENT.md), and run the
installer from a freshly extracted package with `-WhatIf` before any SD write.
Do not attach retail files, SD logs that contain private paths, or SDK files to
an upstream pull request.


## Deferred follow-up after the dev merge (October 11, 2026)

The merge is complete. These items remain open for future work; they do not
imply that the associated features have already passed hardware testing.

- [ ] **Return to Lobby:** preserve the selected map format, map, gametype and
  local match settings. Xbox matches currently return to the main menu;
  PC matches from the shared menu can reopen the separate PC selector with
  different retained choices.
- [ ] **Wireless feature parity:** bots and the expanded touch scoreboard are
  still solo/local features. Networking them requires separate implementation
  and synchronization tests.
- [ ] **PC map thumbnails:** add top-screen images for PC-only maps. Their
  bottom-screen geometry previews already work.
- [ ] **Physical-console validation:** test the merged build on Original and
  New models, including a mixed-model pair. Check Xbox/PC map identity,
  missing-map rejection, match completion/disconnects, co-op gameplay and
  blocked campaign extras, gear previews, and touch-aim capture. Measure
  Balanced/Quality frame times and memory use. Include full-roster bot matches
  and the previously reported Death Island tank-firing path. Emulator results
  are useful coverage, not proof of console performance or wireless stability.
- [ ] **Setup packages:** rebuild and verify the Windows 7 and WinUI packages
  with the final merged executable before distributing the next test release.
  Optional PC-folder import is implemented, but the last packaged executable
  predates the final touch-drag and shared gear-menu changes. Check fresh
  Xbox-only and Xbox-plus-PC setup and executable-only updates.
- [ ] **Documentation cleanup:** reconcile historical branch/release and
  New-3DS-only wording across the imported notes, and make the shared-menu
  instructions consistent. Preserve attribution and license notices.
- [ ] **Performance preset:** implement and benchmark the reserved Performance
  quality option before enabling it. It remains unavailable; Balanced is the
  default and Quality is opt-in.

Completed before the merge: optional retail PC maps-folder import in both
GUI setup tools; full-screen captured touch aiming; and the shared gear page
opened with X or **Gear previews** after map/gametype selection. Gear previews
read the selected Xbox or PC map asynchronously. Xbox maps keep their original
equipment choices, while PC maps also offer Flamethrower/Fuel Rod presets.

Validation for the merged head: Xbox-only and unified builds compile; host
regressions cover settings/input, map identity and co-op gating, wireless
transport, local menu state, asynchronous previews and model readers. All
four GitHub cross-platform setup test/package jobs passed on `6fee7f8`.
Attribution and repository-content checks passed. The model reader decoded
all 63 models (202 selected parts) in the owner's Xbox Blood Gulch cache in
resident and streamed modes. The final New-model Azahar run navigated the
original Xbox menus, rendered pistol and Warthog previews, returned to match
settings, and loaded Blood Gulch into gameplay.

The final executable's SHA-256 is
`1e2f595debd92f166b8058923399cc92c2fbabc572b6e154167050c7d8fb07c3`.
It was verified after copying to the owner's SD card. Merge-only and notes-only
commits do not change that executable. Promotion to `main` and publication of
new release assets remain separate work.

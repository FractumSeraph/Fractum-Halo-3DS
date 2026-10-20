# The fork's port layer, for the 3DS

These are the FractumSeraph/OpenCE fork's `port/linux` headers and sources
(GPL-3.0-or-later): its engine's calls into its port layer (settings,
logs, map families, the network, Delta) are declared and defined here.
The 3DS build reaches them after its own include folders
(`after_include_dirs` in `port/n3ds/sources.json`), so that Halo3DS's
headers win where both have one. Only some of these units are built yet;
the others' functions are stand-ins in `port/n3ds/src/engine_fork_stubs.c`
until each phase of [the plan](../../docs/MULTIPLAYER-PLAN.md) brings its
unit in (the network's first: `src/posix_net.c`, `src/xnet.c`, `src/p2p.c`).

The CRT stand-ins the Linux build needs (its own `stdio`, `string` and
`math` wrappers) are left out: devkitARM's newlib provides them.

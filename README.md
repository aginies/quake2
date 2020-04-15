# quake2 Chaos

Chaos game mode patched to be buildable on x86_64 and compatible with yquake2 game.

From: https://chaoticdreams.org/q2chaos/

please use it with with latest release of yquake2:
https://www.yamagi.org/quake2/

Source code:
https://github.com/yquake2/yquake2

# usage

IE server:
q2ded +set game chaos +dedicated 1 +set public 1 +exec autoexec.cfg

on client:
./quake2 +set game chaos +connect SERVERNAME

# build and installation

make build_release

copy the "gamex86_64" to "QUAKE2DIR/chaos/game.so"

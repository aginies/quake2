# Quake2 Chaos

This Chaos game mode patched to be buildable on x86_64 and compatible with recent Quake2 game.
The current version of Quake2 (3.21) is usable as client only, as there is still some code which 
are not really clean yet.

The original Chaos mod is from [chaoticdreams.org](https://chaoticdreams.org/q2chaos/)

please use it with with a recent release of quake like:
yquake2:
* https://www.yamagi.org/quake2/
* https://github.com/yquake2/yquake2

Q2Pro:
* https://skuller.net/q2pro/
* https://github.com/skullernet/q2pro

# Usage

On server:
'''
./q2proded +set game chaos +dedicated 1 +set public 1 +exec autoexec.cfg
'''

On client:
'''
./quake2 +set game chaos +connect SERVERNAME
'''

# Build and installation

## Linux

'''
make build_release
'''

Copy the ''gamex86_64.so'' to ''QUAKE2DIR/chaos/'' directory. 

## Windows

* install mgw-sdk
* backup the current Makefile
* rename Makefile_mingw to Makefile
* check the .config in the directory (adjust path to mgw-sdk binairies)
* type:
'''
make
....
[LD] gamex86_64.dll
'''



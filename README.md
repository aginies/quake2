# Quake2 Chaos MOD

This **Chaos mod** patched to be buildable on x86_64 and compatible with recent Quake2 game.
The current version of Quake2 (3.21) is usable as client only, as there is still some code which 
are not really clean yet. The code is in the chaossource directory.

The original **Chaos** mod is from [chaoticdreams.org](https://chaoticdreams.org/q2chaos/)

Please use it with with a recent release of quake2 improvment projects.

**yquake2**:
* [yamagi.org](https://www.yamagi.org/quake2/)
* [yquake2 source code:](https://github.com/yquake2/yquake2)

**Q2Pro**:
* [q2pro](https://skuller.net/q2pro/)
* [q2pro source code:](https://github.com/skullernet/q2pro)

# Usage

On server:
```
./q2proded +set dedicated 1 +set game chaos \
    +set hostname ginies.org  +set public 1 \
    +set allow_download 1 +exec autoexec.cfg \
    +set sys_console 2 +set sv_status_show 2
```

On client:
```
./quake2 +set game chaos +connect SERVERNAME
```

# Build and installation

## Linux

```
make build_release
```

Copy the ''gamex86_64.so'' to ''QUAKE2DIR/chaos/'' directory. 

## Windows

* install mgw-sdk
* backup the current Makefile
* rename Makefile_mingw to Makefile
* check the .config in the directory (adjust path to mgw-sdk binairies)

```
make
....
[LD] gamex86_64.dll
```



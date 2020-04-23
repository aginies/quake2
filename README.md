# Quake2 Chaos MOD

This **Chaos mod** patched to be buildable on x86_64 and compatible with recent Quake2 game.
This is based on the latest [source code]i(https://files.chaoticdreams.org/Chaos/ChaosDM/chaosdm_114b4src.zip) available.
The current version of Quake2 (3.21) is usable as client only, as there is still some code which 
are not really clean yet. The code is in the chaossource directory.

The original **Chaos** mod is from [chaoticdreams.org](https://chaoticdreams.org/q2chaos/)

![](images/chaos.jpg)

You need the [Chaos pack files and maps](https://files.chaoticdreams.org/Chaos/ChaosDM/q2chaos.exe) (this is a self autoextract rar file), and put them into a chaos directory, inside the quake2 directory.
You can find more original source at [files.chaoticdreams.org](https://files.chaoticdreams.org/Chaos/ChaosDM/)

Please use this mod with a recent release of quake2 projects, especially the server side, ie:

**yquake2**:
* [yamagi.org](https://www.yamagi.org/quake2/)
* [yquake2 source code](https://github.com/yquake2/yquake2)

**Q2Pro**:
* [q2pro](https://skuller.net/q2pro/)
* [q2pro source code](https://github.com/skullernet/q2pro)


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
./q2pro +set game chaos +connect SERVERNAME +exec autoexec.cfg
```

# Binairies release

linux and Windows pre-built libs.
Bunzip and copy to ``QUAKE2/chaos`` directory


## chaos_bin/original_V1.15/

Orignal version of Chaos.

## chaos_bin/devel_V1.15.1/

Version with more addons like more weapons and items.

![](images/plasma.jpg)

# Make your own Build and installation

## Linux

```
make build_release
```

Copy the ``gamex86_64.so`` to ``QUAKE2DIR/chaos/`` directory. 

## Windows

* install mgw-sdk
* backup the current Makefile
* use the ``Makefile_mingw` file
* check the ``.config`` in the directory (adjust path to mgw-sdk binairies)
* build the dll:
```
make -f Makefile_mingw
....
[LD] gamex86_64.dll
```
* copy the ``gamex86_64.dll`` to ``QUAKE2DIR/chaos/`` directory.

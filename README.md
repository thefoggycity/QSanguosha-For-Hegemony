Open Source Sanguosha (Hegemony-mod)
==========

Original Mogara Repository
----------

[Original README](./README_Mogara.md)

| Homepage:      | http://qsanguosha.org                        |
|----------------|-----------------------------------------------|
| API reference: | http://gaodayihao.github.com/QSanguosha/api   |
| Documentation: | https://github.com/gaodayihao/QSanguosha/wiki (Chinese) |

Fork Adaption by thefoggycity
----------

- Bug fixes and adapted for later Qt versions (5.13 tested)
- Added server CLI-mode support
- Added docker builder scripts

Docker Mode Usage
----------

Refer to [docker image readme](./docker_build/QSanguosha-Hegemony-Server-Docker/README.md).

Build Instructions
----------

### Environment Prerequisites

- C++ compiler and GNU Make
- Qt 5 development tools
- SWIG code generator

Using apt (Debian-based, e.g. Ubuntu):
```
sudo apt install build-esential qt5-default qttools5-dev-tools swig
```

Using dnf (RHEL/CentOS/Fedora):
```
sudo dnf install qt5-qtbase-devel qt5-qttools-devel make swig
cd /usr/bin/    # Run "which qmake-qt5" to confirm the directory
sudo ln -s lrelease-qt5 lrelease
sudo ln -s qmake-qt5 qmake
```

### Build Process (Linux)

1. Clone the repository, path referred as `$REPO`.
2. Decide your build target by editing `$REPO/QSanguosha.pro`. 
  - Server: remove `audio` from `CONFIG`, but add `serveronly`.
  - Client: remove `serveronly` from `CONFIG`. If audio is desired, add `audio`.
3. Generate Lua interface code using SWIG:
  - `cd $REPO/swig` and execute `swig -c++ -lua sanguosha.i`. This will generate `sanguosha_wrap.cxx` in that directory.
  - Known issue: in function `QString LuaViewAsSkill::getExpandPile() const`, the `return false;` should be changed to `return NULL;`.
4. If audio support is enabled in step 2, install the audio library.
  - Base on your CPU architecture, find FMOD library by `cd $REPO/lib/linux/[x86 or x64]/`.
  - Run `sudo cp libfmod*.so /usr/lib/` followed by `sudo ldconfig` to install.
5. Create build directory `$REPO/../build` alongside the repository.
6. `cd $REPO/../build` and invoke `qmake ../$REPO/`.
7. Run `make` in the build directory.
8. Copy the built binary `QSanguosha` to `$REPO`.
9. Now the application can be run from there.

### Windows and MacOS

Refer to the [original README](./README_Mogara.md).

Deployment
----------

### Linux

1. Copy or create the following files and folders in the deployment folder, called `$APP`.
  - `QSanguosha` is the executable just built.
  - `lib/` and `qt-plugins` are empty directories need to be created.
  - (Optional) `LICENSES/` contains the four license text files in the repository.
  - Other resources files can be directly copied from the repository.
```
ai-selector/    font/       LICENSES/    rule/
audio/          hero-skin/  lua/         sanguosha.qm
diy/            image/      QSanguosha   skins/
extension-doc/  lang/       qt-plugins/  style-sheet/
extensions/     lib/        qt_zh_CN.qm  ui-script/
```

2. Copy the following items from system library path (`/lib/x86_64-linux-gnu/` for example) to `$APP/lib/`
```
libdouble-conversion.so.3  libQt5Core.so.5     libQt5Svg.so.5
libfmodexL.so              libQt5DBus.so.5     libQt5Widgets.so.5
libfmodex.so               libQt5Gui.so.5      libQt5XcbQpa.so.5
libpcre2-16.so.0           libQt5Network.so.5
```
The required shared libraries depend on build. Hence, these files should be retrieved from the development machine. Please consider using `ldd <executable>` and `lsof -p <process_id>` (at runtime) to check the required libraries. The Python script [ldd_copy](./ldd_copy.py) uses `ldd` to gather shared libraries, but please be aware that many of them comes with virtually all linux releases.

3. Copy the following folders from system Qt 5 library path (`/lib/x86_64-linux-gnu/qt5/plugins/` for example) to `$APP/qt-plugins/`.
```
bearer/        platforminputcontexts/  platformthemes/
imageformats/  platforms/              xcbglintegrations/
```

4. Create a starter script `start.sh` in `$APP` for application start-up. The script should add `./lib` to `LD_LIBRARY_PATH`. An example will be
```
#!/bin/sh
env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib ./QSanguosha
```
If your screen is Hi-DPI (e.g. 2K and above) where the application appears small, use
```
env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib QT_AUTO_SCREEN_SCALE_FACTOR=1 ./QSanguosha
```

5. (Optional) Create a desktop file as application launcher with icon.

### Windows

1. Follow the 1st step for deployment in Linux (executable is `QSanguosha.exe`). In addition, copy `fmodex64.dll` and `fmodexL64.dll` as well.

2. Copy the following items from system Qt library path (`C:\Qt\[version]\bin\` for example) to `$APP`.
```
Qt5Core.dll  Qt5Gui.dll  Qt5Network.dll  Qt5Qml.dll  Qt5Quick.dll  Qt5Widgets.dll
```

3. Find `msvcr120.dll` and `msvcp120.dll`, copy them to `$APP`.

4. Copy the following items from system Qt plugin path (`C:\Qt\[version]\plugins\` for example) to `$APP\qt-plugins`.
```
bearer\qgenericbearer.dll  platforms\qwindows.dll  styles\qwindowsvistastyle.dll
imageformats\[qgif, qicns, qico, qjepg, qtga, qtiff, qwbmp, qwebp].dll
```

5. (Optional) On Hi-DPI display, create a launch script `QSanguosha_hidpi.bat` in `$APP`
```
set QT_AUTO_SCREEN_SCALE_FACTOR=1
.\QSanguosha.exe -platform windows:dpiawareness=0
```

License
----------

### Code
This game is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3.0
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

See the LICENSE file for more details.

### Material
Our Materials are under the terms of the Creative Commons
Attribution-NonCommercial-NoDerivatives 4.0 International (CC
BY-NC-ND 4.0)

**You are free to:**

Share — copy and redistribute the material in any medium or format

**Under the following terms:**

Attribution — You must give appropriate credit, provide a link to
the license, and indicate if changes were made. You may do so in
any reasonable manner, but not in any way that suggests the licensor
endorses you or your use.

NonCommercial — You may not use the material for commercial purposes.

NoDerivatives — If you remix, transform, or build upon the material,
you may not distribute the modified material.

See the CC BY-NC-ND 4.0 file for more details.


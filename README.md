# B.Silent

Worried about the daily life noise level? But you only want a bit - silence? 
Then B.Silent is the virtual instrument to make your dreams come true.

B.Silent is a unique MIDI instrument to produce silence in highest quality.
Choose between 10 hand-selected types of silence provided as built-in presets
to play silence in a never heard cleanliness. No background noise, no 
distortion, no glitches, only pure silence.

![screenshot](https://github.com/sjaehn/BSilent/blob/master/doc/screenshot.png "Screenshot from B.Silent")

Enjoy the silence!
Released on April 1, 2023


## Installation

Choose one of the following options:

a) Install the bsilent package for your system (once established)

b) Use the latest provided binaries

Unpack the provided bsilent-\*.zip or bsilent-\*.tar.xz from the latest release and 
copy the BSilent.lv2 folder to your lv2 directory (depending on your system settings,
~/.lv2/, /usr/lib/lv2/, /usr/local/lib/lv2/, or ...).

c) Build your own binaries in the following three steps.

Step 1: [Download the latest published version](https://github.com/sjaehn/BSilent/releases) of B.Silent. Or clone or
[download the master](https://github.com/sjaehn/BSilent/archive/master.zip) of this repository including the submodules. For git cloning use:
```
git clone --recurse-submodules https://github.com/sjaehn/BSilent.git
```

Step 2: Install pkg-config and the development packages for x11, cairo, and lv2 if not done yet. If you
don't have already got the build tools (compilers, make, libraries) then install them too.

On Debian-based systems you may run:
```
sudo apt-get install build-essential
sudo apt-get install pkg-config libx11-dev libcairo2-dev lv2-dev
```

On Arch-based systems you may run:
```
sudo pacman -S base-devel
sudo pacman -S pkg-config libx11 cairo lv2
```

Step 3: Building and installing into the default lv2 directory (/usr/local/lib/lv2/) is easy using `make` and
`make install`. Simply call:
```
make
sudo make install
```

**Optional:** Standard `make` and `make install` parameters are supported. You may build a debugging version 
using `make CXXFLAGS+=-g`. For installation into an alternative directory (e.g., /usr/lib/lv2/), change the
variable `PREFIX` while installing: `sudo make install PREFIX=/usr`. If you want to freely choose the
install target directory, change the variable `LV2DIR` (e.g., `make install LV2DIR=~/.lv2`).


## Running

After the installation Ardour, Carla, and any other LV2 host should automatically detect B.Silent.

If jalv is installed, you can also call it using one of the graphical jalv executables (like
jalv.gtk, or jalv.gtk3, or jalv.qt4, or jalv.qt5, depending on what is installed), like

```
jalv.gtk https://www.jahnichen.de/plugins/lv2/BSilent
```

to run it stand-alone and connect it to the JACK system.
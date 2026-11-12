NETSPEED - INSTALLATION GUIDE

DESCRIPTION
A plugin for wf-panel-pi.
Displays download and upload speeds on two lines.
Includes customizable colors, units of measurement,
font size, and a reset to defaults option.

REQUIREMENTS
- Linux with a compatible wf-panel-pi
- g++
- pkg-config
- GTKmm 3
- wf-panel-pi development headers

DEPENDENCIES ON RASPBERRY PI OS / DEBIAN
sudo apt update
sudo apt install build-essential pkg-config libgtkmm-3.0-dev wf-panel-pi-dev

INSTALLATION
1. Extract the package.
2. Open a terminal inside the NetSpeed-portatile folder.
3. Run:
   ./build.sh
4. Run:
   ./install.sh
5. Open:
   ~/.config/wf-panel-pi/wf-panel-pi.ini
6. Add netspeed to widgets_right or widgets_left.
7. Restart the panel:
   pkill -TERM -x wf-panel-pi

UNINSTALLATION
Run:
   ./uninstall.sh

Then restart the panel.

SETTINGS
Personal preferences are located at:
~/.config/netspeed-widget.conf

Uninstallation does not delete this file.

IMPORTANT NOTE
The package contains the source code, not a precompiled
library. Compilation must be performed on the target
computer.

NetSpeed is a plugin for wf-panel-pi.
It does not work directly with XFCE Panel, LXPanel,
Waybar, or other generic panels.

Compatibility on Debian, Ubuntu, and Linux Mint depends
on the availability of wf-panel-pi and an API compatible
with the one used to develop the plugin.

The Raspberry Pi ARM64 library cannot be used directly
on Intel/AMD PCs: it must be recompiled locally.

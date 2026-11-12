#!/bin/bash
set -e
cd "$(dirname "$0")"

if [ ! -f libnetspeed.so ]; then
    echo "Prima eseguire ./build.sh"
    exit 1
fi

ARCH=$(gcc -print-multiarch)
PLUGIN_DIR="/usr/lib/$ARCH/wf-panel-pi"

if [ ! -d "$PLUGIN_DIR" ]; then
    echo "Directory wf-panel-pi non trovata:"
    echo "$PLUGIN_DIR"
    exit 1
fi

sudo install -m 755 libnetspeed.so "$PLUGIN_DIR/libnetspeed.so"

echo "NetSpeed installato."
echo
echo "Aggiungere netspeed alla riga widgets_right"
echo "oppure widgets_left in:"
echo "~/.config/wf-panel-pi/wf-panel-pi.ini"
echo
echo "Poi riavviare il pannello:"
echo "pkill -TERM -x wf-panel-pi"

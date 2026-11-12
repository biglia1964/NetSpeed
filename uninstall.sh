#!/bin/bash
set -e

ARCH=$(gcc -print-multiarch)
PLUGIN="/usr/lib/$ARCH/wf-panel-pi/libnetspeed.so"
CONFIG="$HOME/.config/wf-panel-pi/wf-panel-pi.ini"

if [ -f "$CONFIG" ]; then
    cp -a "$CONFIG" "$CONFIG.backup-netspeed"
    sed -i -E '/^widgets_(right|left)=/ {
        s/(^|[[:space:]])netspeed([[:space:]]|$)/ /g
        s/[[:space:]]+/ /g
        s/=$/ =/
        s/[[:space:]]+$//
        s/ =/ =/
    }' "$CONFIG"
fi

sudo rm -f "$PLUGIN"

echo "NetSpeed disinstallato."
echo "Le impostazioni personali sono state conservate."
echo "Riavviare il pannello con:"
echo "pkill -TERM -x wf-panel-pi"

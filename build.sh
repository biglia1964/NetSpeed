#!/bin/bash
set -e
cd "$(dirname "$0")"

echo "Compilazione NetSpeed..."

g++ -std=c++17 -fPIC -shared \
netspeed.cpp -o libnetspeed.so \
-I/usr/include/wf-panel-pi \
$(pkg-config --cflags --libs gtkmm-3.0)

echo "Controllo libreria..."
file libnetspeed.so
nm -D libnetspeed.so | grep ' create$'

if ldd -r libnetspeed.so 2>&1 | grep -E 'not found|undefined symbol'; then
    echo "Errore nelle dipendenze."
    exit 1
fi

echo "Compilazione completata!"

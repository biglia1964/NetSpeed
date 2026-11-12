NETSPEED - GUIDA DI INSTALLAZIONE

DESCRIZIONE
Plugin per wf-panel-pi.
Visualizza download e upload su due righe.
Include colori personalizzabili, unità di misura,
dimensione caratteri e ripristino predefiniti.

REQUISITI
- Linux con wf-panel-pi compatibile
- g++
- pkg-config
- GTKmm 3
- Header di sviluppo wf-panel-pi

DIPENDENZE SU RASPBERRY PI OS / DEBIAN
sudo apt update
sudo apt install build-essential pkg-config libgtkmm-3.0-dev wf-panel-pi-dev

INSTALLAZIONE
1. Estrarre il pacchetto.
2. Aprire il terminale nella cartella NetSpeed-portatile.
3. Eseguire:
   ./build.sh
4. Eseguire:
   ./install.sh
5. Aprire:
   ~/.config/wf-panel-pi/wf-panel-pi.ini
6. Aggiungere netspeed a widgets_right oppure widgets_left.
7. Riavviare:
   pkill -TERM -x wf-panel-pi

DISINSTALLAZIONE
Eseguire:
   ./uninstall.sh

Poi riavviare il pannello.

IMPOSTAZIONI
Le preferenze personali si trovano in:
~/.config/netspeed-widget.conf

La disinstallazione non elimina questo file.

NOTA IMPORTANTE
Il pacchetto contiene il codice sorgente, non una libreria
precompilata. La compilazione deve essere eseguita sul
computer di destinazione.

NetSpeed è un plugin per wf-panel-pi.
Non funziona direttamente con XFCE Panel, LXPanel,
Waybar o altri pannelli generici.

La compatibilità su Debian, Ubuntu e Linux Mint dipende
dalla disponibilità di wf-panel-pi e di una sua API
compatibile con quella usata per sviluppare il plugin.

La libreria ARM64 del Raspberry non è utilizzabile
direttamente su PC Intel/AMD: occorre ricompilarla.

# Pico-UI

A combined loader and terminal for the Pico.

  - Automatically connects to a Pico via USB (uses VID/PID to find it)
  - Runs a terminal window to talk/debug programs on the Pico
  - Can reset the Pico into Boot mode and download a binary (*.UF2) to the target. Will automatically reconnect USB-serial  after download.

## Building
  - Tested with Qt5.15.2 on OpenSuse Leap 15.2/15.4. There is no intend to make it working in other OS. I might or  it might not, I don't care.
  - Created with QtCreator
  - qmake based
  - IBB qt-lib is used, this will later be added to the repo

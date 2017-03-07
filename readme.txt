ARVID - homebrew arcade videocard
=================================
version 0.4e

Arvid is an open source and open hardware videocard with the main goal to
produce perfect TV output for arcade games of the past emulated on modern PCs.

more info about the project:
http://www.volny.cz/molej/arvid


dependencies:
am335x_pru_package software from:
https://github.com/beagleboard/am335x_pru_package
Make sure you can access its header files and libprussdrv.a library.
If you have a BBG with kernel 3.8.13 or so, chances are you will
need no downloads or installation at all as required libraries and
tools are already installed.
As of 2016 it doesn't work on 4.X Linux kernel, so use 3.8.X kernel
which supports PRU.

build:
- build works only on BBG (BBB) mini computer
- to build all just type 'make'

notes:
- libarvid requires superuser access to access PRU and gpios
- libarvid manages setting up cape manager itself
- libarvid manages ddr mapping itself
- works on BBB and BBG but not on older HW as it depends on exact PRU timing (older 
  hardware PRU clock is slower)
- pru binary code (prustream.bin, pruvideo_XXX.bin) have to exist in
  current working directory of an application that uses/links libarvid



Ver 0.4e
-------------
- Added virtual vsync option that allows to minimize
  screen latencies. New function was added :
  arvid_set_virtual_vsync(int vsync_line)
  where vsync_line is the scan line that triggers the
  vsync event. By default the feature is disabled
  (vsync_line = -1). If enabled arvid_wait_for_vsync() function
  will wait until the vsync_line is rendered on the screen
  and then it returns to the caller. If disabled
  arvid_wait_for_vsync() waits until the last line is rendered
  on screen and then it returns to the caller.

Ver 0.4d
-------------
- Added new mid-res video modes 416, 448, 512, 640

- Added automatic screen saver. After 3 minutes of 
  inactivity the screen saver is started. To cancel it
  either a Coin or Start button has to be pressed or
  arvid API function has to be called. For example
  when an emulator is started the screen server is cancelled.
  This feature prevents screen burn when Arvid is idling
  in service screen or when an emulator / app crashed
  while unattended (inside arcade cab).

Ver 0.4c
-------------
- Picture quality enhancement. A timing bug was fixed so now
  the video signal is more stable. Previously every 8-th line
  was slightly shifted to the right causing artefacts.

- Support for firmware upgrade over network. The server can
  receive an upgrade file and store it locally. Physical
  unpacking of the upgrade file and replacement of arvid binaries is
  performed by the Linux OS scripts during Arvid reboot.
  NOTE! The Coin button MUST be pressed when the firmware is about
  to be uploaded. If the coin button is not pressed the uploaded
  file is rejected. It is for the security of your network
  to prevent malicious code to be uploaded to your BBG.

-------------
Ver 0.4b
- added new function to set screen X offset. This allows you to adjust
  the screen left position so it better fits your TV set.
  Use the following functions: arvid_set_line_pos() and arvid_get_line_pos()
  Valid values are 1 - 111 inclusive, where the default value is 60 (which roughly
  corresponds to the previous screen X position). All video modes are affected
  by this modifier the same way, that is all videomodes will be rendered
  at exactly the same X position irrespective of their resolution.

-------------
Ver 0.4a
- internal: simplified and unified videomode source code. It should be easier
  to define new videomode by just setting video parameters.
- added new videomode 336
- added videomode enumeration function
- added videomode test app
- arvid-server now shows the version number on the service screen
-------------
Ver 0.3e
- added support for Start button on pin P9_25. When connected to ground
  arvid_get_button_state() will report the Start button is pressed.
  Make sure to copy the latest *.dtbo to /lib/firmware!
- removed the old 12-bit dts files (these were obsolete long time ago)
-------------
Ver 0.3d
- new video mode 292
- fixed framerate test

-------------
Ver 0.3c
- changed the frame buffer streaming from PRU based to CPU based.
  The reason was to achieve better picture stability. Previously
  very slight screen horizontal shaking was present, which was 
  caused by PRU jitter when it copied DDR memory. CPU based mem copy
  doesn't cause jitter, but increases the CPU load significantly.
- remove 'wicd' as it causes screen flickering now and then
  sudo apt-get purge wicd*
- identify running services by using:
  systemctl list-units --type service
-------------
Ver 0.3b
- changed sync pin to p8.28 to fix startup issues
- added reading of buttons on  GPIO3 (requires dtbo update)
  new buttons: 
   * tate switch p9.27
   * coin button p9.28
  all buttons need to be connected to ground to report value 1.
  Internally they are pulled high.
- dtbo now supports Beaglebone Green as well 
- added arvid-splash program that shows arvid logo during boot-up
- improved arvid-server to show ipaddress of active network interface.
  Previously ipaddress of eth0 was always displayed even when it was not
  connected.
- improved service screen to show rotated logo according the state of the
  Tate switch.

-------------
Ver 0.3a
- now switched to 15bit graphics (previously 12 bit)
- new video modes 240, 392, 400
- requires gpio rewiring and new dtbo!

-------------
Ver 0.2c
- support for packet duplicates to tackle lost packets issue


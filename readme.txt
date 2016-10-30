ARVID - homebrew arcade videocard
=================================

Arvid is an open source and open hardware videocard with the main goal to 
produce perfect TV output for arcade games of the past emulated on modern PCs.

more info about the project:
http://www.volny.cz/molej/arvid


dependencies:
am335x_pru_package software from:
https://github.com/beagleboard/am335x_pru_package
Make sure you can access its header files and libprussdrv.a library.

build:
build works only on BBB mini computer
to build all just type 'make'

notes:
- libarvid requires superuser access to access PRUSS and gpios
- libarvid manages setting up cape manager itself
- libarvid manages ddr mapping itself
- works on BBB and BBG but not on older HW as it depends on exact PRU timing (older 
  hardware PRU clock is slower)
- pru binary code (prustream.bin, pruvideo_XXX.bin) have to exist in
  current working directory of an application that uses/links libarvid

-------------
Ver 0.4a
- internal: simplified and unified videomode sourcecode. It should be easier
  to define new videomode by just setting video parameters.
- added new videomode 336
- added videomode enumeration function
- added videomode test app
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


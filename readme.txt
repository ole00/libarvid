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
- works only on BBB not on older HW as it depends on exact PRU timing (older 
  hardware PRU clock is slower)
- pru binary code (prustream.bin, pruvideo_XXX.bin) have to exist in
  current working directory of an application that uses/links libarvid


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


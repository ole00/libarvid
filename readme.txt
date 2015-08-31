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
Ver 0.2c
- support for packet duplicates to tackle lost packets issue


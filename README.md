OpenChronosSWAP
===============

This is an up to date port of the OpenChronos with 
SWAP support with the current msp430 lts and some bug fixes.

Porting of the original code from Daniel Berenguer at Panstamp:
https://github.com/dberenguer/OpenChronos


== Status ==

Should be fully functional. 
Please note that the simpliciti protocol and so the rfbsl is totally
eradicated.
The rfbsl is a binary blob provided by BMh and is based on the
BlueRobin algorithm.
At the moment no wireless update is supported.

== Requirements ==

mspgcc LTS 20120406 
http://sourceforge.net/apps/mediawiki/mspgcc/index.php?title=MSPGCC_Wiki 

make
python http://python.org

== Supported Compilers ==

   Working combinations:

gcc version 4.6.3 20120301 (mspgcc LTS 20120406 unpatched) (GCC) 
   gcc=4.6.3 binutils=20120224 libc=20120224

== HOWTO ==


To configure your image, run:

   make config

which will generate a config.h file that contains the settings for your build.

To compile the image run:

   make


It is HIGHLY suggested to make a clean build before you flash the image with:

   make clean main

== Problems ==

 * Accelerometer and PPT mode are working over SWAP, I have to re-do the GUI on the receiver side

== Difference to the TI Firmware ==

* Lots!

== Usefull Tools ==
 * mspdebug - debugger, proxy for gdb, flasher, ...  
     http://mspdebug.sourceforge.net
 * msp430static - static analyzer for sourcecode
     http://msp430static.sourceforge.net/
   run 'make source_index' to index all source files


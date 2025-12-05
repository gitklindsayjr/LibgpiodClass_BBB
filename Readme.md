This software was created for imbedding in applications that require the use of the GPIO interface on Debian based Linux SOC's. 

This software expects that the following pins:<br>
P8_10, P8_12, P8_14, P8_17, P8_18, P8_26, P9_12 to be mapped
The include file that holds the information is "BBB_Gpiod.h" in a array named "gpio_lines_t linesArray".

The Beaglebone Black was the SOC for which this Class was initially developed. The Linux enviroment is:<br>
image  -- Beaglebone Black Debian 13 2025-09-05 - am335c-debian-13-base-v6.12-armhf-2025-09-05-4gp.img.xz<br>
kernel -- version 6.12.49-bone36 Sep 25 19:56:57 UTC 2025 armv7l GNU/Linux

This software requires the installation of the gpiod library on a Beaglebone Black running the above kernel.<br>

$ sudo apt update<br>
$ sudo apt install build-essential<br>
$ sudo apt install g++ gcc cmake<br>
$ sudo apt install libgpiod-dev<br>

Initial development was cross compiled from a Ubuntu 20.04 PC.  The shared library, include files came from downloading the:<br>
https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/libgpiod-2.2.2.tar.gz file.

Install software on the Beaglebone Black:<br>
$ git clone http://github.com/gitklindsayjr/CmakeClass_tests

The following should be the result of the clone operation<br>
$ cd GpiodClass_test<br>
$ ls<br>
CMakeLists.txt src<br>
$ ls src<br>
BBB_Gpiod.cpp BBB_Gpiod.h gpiod_class.cpp gpiod_class.h gpiod_structs.h main.cpp<br>

Begin the build process:<br>
$ mkdir build<br>
$ cmake -DNATIVE=1 ..<br>
$ cmake --build . -- VERBOSE=1<br>

Build process results:<br>
$ ls
CMakeCache.txt CMakeFiles Makefile cmake_install.cmake gpiod-test

The executable "gpiod-test" uses P8_12 and P8_14 as inputs and P8_10 and 17 for output. A waveform generator with a 5Hz "square wave" is connected to P8_12 and P8_14.  This will test the following functions "int testEdgeEvents(Gpiod gpiod)" and the :int testPulseTiming(Gpiod gpiod)". A square wave runing at approximately 100Hz is outputed on P8_10 and P8_17.

$ ./gpiod-test
	

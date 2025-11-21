This software was created for imbedding in applications that require the use of the GPIO interface on Debian based Linux SOC's. 

This software expects that the following pins:
	P8_10, P8_12, P8_14, P8_17, P8_18, P8_26, P9_12 to be mapped
The include file that holds the information is "BBB_Gpiod.h" in a array named "linesArray" of the following type

typedef struct
{
	unsigned  lineId;
	unsigned  lineNum;
	gpiod_chip_t *chip;
	gpiod_settings_t *settings;
	gpiod_request_t *request;
	string chipPath;
	string lineName;
	string consumer;
} gpio_lines_t;

The Beaglebone Black was the SOC for which this Class was initially developed. The Linux enviroment is:
	image  -- Beaglebone Black Debian 13 2025-09-05 - am335c-debian-13-base-v6.12-armhf-2025-09-05-4gp.img.xz
	kernel -- version 6.12.49-bone36 Sep 25 19:56:57 UTC 2025 armv7l GNU/Linux

This software requires the installation of the gpiod library on a Beaglebone Black running the above kernel.

$ sudo apt update
$ sudo apt install build-essential
$ sudo apt install g++ gcc cmake
$ sudo apt install libgpiod-dev

Initial development was cross compiled from a Ubuntu 20.04 PC.  The shared library, include files came from downloading the:
	https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/libgpiod-2.2.2.tar.gz file.

The extracted file was installed in the following directory on my PC. The CMakeList.txt supports both options.
	~/MicroControllers/Gpiod-Lib
		|	
		include
			|
			gpiod.h
			gpiod.hpp
			gpiodcxx
		lib
			|
			aarch64-linux-gnu
			arm-linux-gnueabihf
				|
				libgpiod.so
		libgpiod-2.2.2

Install software on the Beaglebone Black:
$ git clone http://github.com/gitklindsayjr/CmakeClass_tests

The following should be the result of the clone operation
$ cd GpiodClass_test
$ ls
	CMakeLists.txt src
$ ls src
	BBB_Gpiod.cpp BBB_Gpiod.h gpiod_class.cpp gpiod_class.h gpiod_structs.h main.cpp

Begin the build process	
$ mkdir build
$ cmake -DNATIVE ..
$ cmake --build . -- VERBOSE=1

Build process results, the excutable will be gpio-test
$ ls
	CMakeCache.txt CMakeFiles Makefile cmake_install.cmake gpiod-test

The executable "gpiod-test" uses P8_12 and P8_14, a "square wave" signal is connected to these pins for the edge and timing tests the call to the "testOutputPulse" should be commented out when the "square wave" signal is applied.

$ ./gpiod-test
	

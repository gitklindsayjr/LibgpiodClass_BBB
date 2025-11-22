/*
 * Copyright (c) 2025, Kenneth Lindsay
 * All rights reserved.
 * Author: Kenneth Lindsay
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *      following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *      following disclaimer in the documentation and/or other materials provided with the distribution.
 *   3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Beaglebone Black Debian 13 2025-09-05 - am335c-debian-13-base-v6.12-armhf-2025-09-05-4gp.img.xz
// Beaglebone kernel version 6.12.49-bone36 Sep 25 19:56:57 UTC 2025 armv7l GNU/Linux

#ifndef BBB_GPIOD_H_
#define BBB_GPIOD_H_

#ifdef NATIVE
#include <gpiod.h>
#else
	#include "gpiod.h"
#endif
#include "gpiod_structs.h"
#include "gpiod_class.h"

/* Default pins - The enumerated list of pins were discovered on this relase version "trixie" using the following tools:
 * $ gpioinfo P8_10 P8_12 P8_14 P8_17 P8_18 P8_26 P9_12
 * gpiochip0 12  "P8_12" input
 * gpiochip0 28  "P9_12" input
 * gpiochip0 29  "P8_26" input
 * gpiochip1 1   "P8_18" input
 * gpiochip1 4   "P8_10" input
 * gpiochip3 26  "P8_14" input
 * gpiochip3 27  "P8_17" input
 * Other pins are availble and can be added to the enumerated list
*/
enum line_id_t_  { P8_10=0, P8_12, P8_14, P8_17, P8_18, P8_26, P9_12, GPIO_NUM_LINES };

#define DUMMY_CHIP_STR "/dev/gpiochipxx"

class Gpiod : public GpiodClass
{
public:
	Gpiod(void);
	~Gpiod(void);
private: // Helper methods
	int  gpioConstructor(void);
	public: // Unique
		gpio_lines_t linesArray[GPIO_NUM_LINES] = {
				{P8_10, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P8_10", ""},
				{P8_12, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P8_12", ""},
				{P8_14, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P8_14", ""},
				{P8_17, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P8_17", ""},
				{P8_18, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P8_18", ""},
				{P8_26, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P8_26", ""},
				{P9_12, 0, NULL, NULL, NULL, DUMMY_CHIP_STR, "P9_12", ""} };
};
#endif /* BBB_GPIOD_H_ */

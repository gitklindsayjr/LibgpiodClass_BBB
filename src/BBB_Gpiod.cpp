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

#include <cstring>
#include "BBB_Gpiod.h"

/* This Constructor initializes the "gpio_lines_t linesArray" with the line number, line name, and chipname similar
 * to that was discovered using the command line "gpioinfo". Other info for the array items come from actual use.
 * A unique "consumer" string is required for device resource conflicts which are handled by the kernel.
 *
*/
Gpiod::Gpiod(void)
{
	GpiodClass::lines = linesArray;
	numLines = GPIO_NUM_LINES;
	gpioConstructor();
}
int Gpiod::gpioConstructor(void)
{
	int ret;
    srand(time(0)); // Seed the random number generator
    unsigned randNum = rand(); // Create a unique consumer
	unsigned lineNum;
	char chipPath[] = "/dev/gpiochipxx";
	char lineName[] = "PX_NN";

	for(unsigned i=0; i<GPIO_NUM_LINES; i++)
	{   // Search for a match in our lines array
		strcpy(lineName, linesArray[i].lineName.c_str());
		ret = findLineByName(chipPath, lineName, &lineNum);
		if(i != linesArray[i].lineId)
		{
			cerr << "ERROR: Line enumerations are out of order" << endl;
			return ERROR;
		}
		else if(ret < 0)
		{
			cerr << "ERROR: Invalid line number" << endl;
			return ERROR;
		}
		numLines = i;
		linesArray[i].chipPath = chipPath;
		linesArray[i].lineNum = lineNum;
		linesArray[i].consumer = to_string(randNum);
	}
	return false;
}
Gpiod::~Gpiod(void)
{
}

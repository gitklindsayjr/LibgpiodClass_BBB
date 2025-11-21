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

#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "BBB_Gpiod.h"

using namespace std;

int testSettingsFunctions(Gpiod gpiod, unsigned lineId);
int testEdgeEvents(Gpiod gpiod, unsigned lineId_P8_14, unsigned lineId_P8_12);
int testPulseTiming(Gpiod gpiod, unsigned lineId);
int testOutputPulse(Gpiod gpiod, unsigned lineId);
int testArrayFunctions(Gpiod gpiod, unsigned *lineId, unsigned *lineNum, string *lineName, string *chipPath, string *consumer);

int main(void)
{
	Gpiod gpiod;
	string chipPath;
	string lineName;
	string consumer;
	unsigned lineNum, lineId_P8_12, lineId_P8_14;
	int error;

	error = gpiod.openLine(P8_14);
	lineNum = gpiod.get_LineNum("P8_14", &error);
	lineId_P8_14 =   gpiod.get_LineId(lineNum, &error);
	error = gpiod.openLine(P8_12);
	lineNum = gpiod.get_LineNum("P8_12", &error);
	lineId_P8_12 =   gpiod.get_LineId(lineNum, &error);
	gpiod.closeLine(P8_14);
	gpiod.closeLine(P8_12);
	error = testArrayFunctions(gpiod, &lineId_P8_14, &lineNum, &lineName, &chipPath, &consumer);
	error = testSettingsFunctions(gpiod, lineId_P8_14);
	error = testPulseTiming(gpiod, lineId_P8_14);
	error = testEdgeEvents(gpiod, lineId_P8_14, lineId_P8_12);
    error = testOutputPulse(gpiod, lineId_P8_14);

	return error;
}
int testArrayFunctions(Gpiod gpiod, unsigned *lineId, unsigned *lineNum, string *lineName, string *chipPath, string *consumer)
{
	gpiod.openLine(P8_14);
	int error = NO_ERROR;
	*lineNum = gpiod.get_LineNum("P8_14", &error);
	*lineNum = gpiod.get_LineNum(P8_14, &error);
	*lineId =   gpiod.get_LineId(*lineNum, &error);
	*lineName = gpiod.get_LineName(*lineId, &error);
	*lineId =   gpiod.get_LineId(*lineName, &error);
	*chipPath = gpiod.get_ChipPath(*lineId, &error);
	*chipPath = gpiod.get_ChipPath(*lineName, &error);
	*consumer = gpiod.get_Consumer(*lineId, &error);
	error = gpiod.set_Consumer(*lineId, "GpiodClass");
	*consumer = gpiod.get_Consumer(*lineName, &error);
	printf("Test array functions success\n");
	return error;
}
int testOutputPulse(Gpiod gpiod, unsigned lineId)
{   // This test is output on P8_14, at approximately 1khz, non real time
	gpiod.openLine(P8_14);
	int value;
	int error = gpiod.setLineDirection(lineId, OUTPUT, LOW);
	if(error < 0)
		return error;
	for(int i=0; i<10000; i++) // Run for 10 seconds
	{
		error = gpiod.setLineValue(lineId, HIGH);
		value = gpiod.getLineValue(lineId);
		usleep(500);
		error = gpiod.setLineValue(lineId, LOW);
		value = gpiod.getLineValue(lineId);
		if(error < 0)
			return error;
		usleep(500);
	}
	printf("Test ouput pulse, 1Khz sq. wave success\n");
	return NO_ERROR;
}
int testPulseTiming(Gpiod gpiod, unsigned lineId)
{   // This test requires a 50HZ sq. wave input on P8_14
	gpiod.openLine(P8_14);
	int n = 0;
    struct timespec start_time, end_time;
    int elapsed_nanoseconds, elapsed_us;
    unsigned total_us = 0;
	int value = LOW;
	if(gpiod.setLineDirection(lineId, INPUT, LOW) < 0)
		return ERROR;
	while(n < 100)
	{
	    clock_gettime(CLOCK_MONOTONIC, &start_time);
		while(value != HIGH) value = gpiod.getLineValue(lineId);
		printf("VALUE: %d\n", value);
		while(value != LOW) value = gpiod.getLineValue(lineId);
		printf("VALUE: %d\n", value);
	    clock_gettime(CLOCK_MONOTONIC, &end_time);
	    elapsed_nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
	    // Handle negative nanoseconds
	     if (elapsed_nanoseconds < 0)
	         elapsed_nanoseconds += 1000000000; // Add 1 billion nanoseconds
	     elapsed_us = elapsed_nanoseconds/1000;
	     total_us+= elapsed_us;
		 n++;
	}
	elapsed_us = total_us/n;
	printf("Test pulse timing of sq. waveform input, success\n");
	printf("Elapsed time in us = %d, average time = %d, with (n) samples = %d\n", elapsed_us, elapsed_us/n, n);
	return NO_ERROR;
}

int testSettingsFunctions(Gpiod gpiod, unsigned lineId)
{
	gpiod.openLine(P8_14);
	bool activeLow = false;
	unsigned direction;
	int value = LOW;
	int error;

	error = gpiod.setLineDirection(lineId, INPUT, LOW);
	error = gpiod.getLineDirection(lineId, &direction);
	if(direction == OUTPUT)
		printf("Direction: OUTPUT\n");
	else
		printf("Direction: INPUT\n");
	gpiod.setLineActiveState(lineId, true);
	error = gpiod.getLineActiveState(lineId, &activeLow);
	if(activeLow)
		printf("Active state: LOW True\n");
	else
		printf("Active state: HIGH True\n");
	value = gpiod.getLineValue(lineId);
	printf("VALUE: %d\n", value);
	value = gpiod.getLineValue(lineId);
	printf("VALUE: %d\n", value);

	gpiod.setLineActiveState(lineId, false);
	error = gpiod.getLineActiveState(lineId, &activeLow);
	if(activeLow)
		printf("Active state: LOW True\n");
	else
		printf("Active state: HIGH True\n");
	value = gpiod.getLineValue(lineId);
	printf("VALUE: %d\n", value);
	value = gpiod.getLineValue(lineId);
	printf("VALUE: %d\n", value);

	error = gpiod.setLineDirection(lineId, OUTPUT, LOW);
	error = gpiod.setLineValue(lineId, HIGH);
	error = gpiod.setLineValue(lineId, LOW);
	value = gpiod.getLineValue(lineId);
	printf("VALUE: %d\n", value);

	gpiod.setLineActiveState(lineId, true);
	error = gpiod.getLineActiveState(lineId, &activeLow);
	if(activeLow)
		printf("Active state: LOW True\n");
	else
		printf("Active state: HIGH True\n");
	value = gpiod.getLineValue(lineId);
	printf("VALUE: %d\n", value);

	error = gpiod.setLineValue(lineId, HIGH);
	error = gpiod.setLineValue(lineId, LOW);
	if(error < 0)
		return error;
	printf("Settings test success\n");
	return error;
}

static const char *edge_event_type_str(unsigned type)
{
	switch(type)
	{
	case GPIOD_EDGE_EVENT_RISING_EDGE:
		return "Rising";
	case GPIOD_EDGE_EVENT_FALLING_EDGE:
		return "Falling";
	default:
		return "Unknown";
	}
}
int testEdgeEvents(Gpiod gpiod, unsigned lineId_P8_14, unsigned lineId_P8_12)
{   // This test requires either manual input to P8_14 and P8_12 or waveform generator signal
	gpiod.openLine(P8_14);
	gpiod.openLine(P8_12);
	enum gpiod_line_edge edge;
	uint64_t  time1_12_ns, time0_12_ns, deltaTime_12_ns = 0;
	uint64_t  time1_14_ns, time0_14_ns, deltaTime_14_ns = 0;
	double deltaTime_14_ms, totalTime_14_ms = 0.0;
	double deltaTime_12_ms, totalTime_12_ms = 0.0;
	int ret_14, ret_12, error = NONE;

	edge = GPIOD_LINE_EDGE_RISING;
	error = gpiod.initLineEdgeEvents(lineId_P8_14, edge);
	edge = GPIOD_LINE_EDGE_FALLING;
	error = gpiod.initLineEdgeEvents(lineId_P8_12, edge);
	if(error < 0)
		return -1;
	unsigned n = 0;
	while(n<100)
	{
		/* Blocks until at least one event is available. */
		ret_14 = gpiod.waitForLineEdgeEvent(lineId_P8_14);
		if(ret_14 < 0)
			return -1;
		for(int i = 0; i < ret_14; i++)
		{
			error = gpiod.getLineEventData(lineId_P8_14, i);
			if(error < 0)
				return -1;
			time1_14_ns = gpiod.event->timestamp;
			deltaTime_14_ns = time1_14_ns - time0_14_ns;
			deltaTime_14_ms = (double)deltaTime_14_ns;
			deltaTime_14_ms/= 1000000;
			time0_14_ns = time1_14_ns;
			printf("offset: %d  type: %-7s period (ms): %7.3f event #%ld\n",
			       gpiod.event->line_offset,
				   edge_event_type_str(gpiod.event->event_type),
				   deltaTime_14_ms,
			       gpiod.event->line_seqno);
			if(deltaTime_14_ms > 100000) // Throw out sample linux is not real time os
				continue;
			if(n>1)
				totalTime_14_ms+= deltaTime_14_ms;
		}
		ret_12 = gpiod.waitForLineEdgeEvent(lineId_P8_12);
		if(ret_12 < 0)
			return -1;
		for(int i = 0; i < ret_12; i++)
		{
			error = gpiod.getLineEventData(lineId_P8_12, i);
			if(error < 0)
				return -1;
			time1_12_ns = gpiod.event->timestamp;
			deltaTime_12_ns = time1_12_ns - time0_12_ns;
			deltaTime_12_ms = (double)deltaTime_12_ns;
			deltaTime_12_ms/= 1000000;
			time0_12_ns = time1_12_ns;
			printf("offset: %d  type: %-7s period (ms): %7.3f event #%ld\n",
			       gpiod.event->line_offset,
				   edge_event_type_str(gpiod.event->event_type),
				   deltaTime_12_ms,
			       gpiod.event->line_seqno);
			if(n>1)
				totalTime_12_ms+= deltaTime_12_ms;
		}
		n++;
	}
	printf("Total time (seconds): %7.3f, Events: %d\n", totalTime_14_ms/n, n);
	printf("Total time (seconds): %7.3f, Events: %d\n", totalTime_12_ms/n, n);
	return 0;
}

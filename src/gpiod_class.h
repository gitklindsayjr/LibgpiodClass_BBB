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


#ifndef GPIOD_CLASS_H_
#define GPIOD_CLASS_H_

#include <string>
#include <iostream>
#include <fstream>
#include "gpiod_structs.h"

using namespace std;

#define ERROR                  -1
#define NO_ERROR               0
#define EVENT_BUFFER_SIZE      2

enum direction_ { OUTPUT=0, INPUT=1 };
enum active_    { HI_TRUE=0, LO_TRUE=1 };
enum enable_    { ENABLE=0, DISABLE=1 };
enum putypesel_ { PULL_DOWN=0, PULL_UP=1 };
enum slewctrl_  { FAST=0, SLOW=1 };
enum state_     { LOW=0, HIGH=1 };

typedef struct gpiod_chip              gpiod_chip_t;
typedef struct gpiod_line              gpiod_line_t;
typedef struct gpiod_line_settings     gpiod_settings_t;
typedef struct gpiod_line_config       gpiod_config_t;
typedef struct gpiod_line_request      gpiod_request_t;
typedef struct gpiod_request_config    gpiod_req_cfg_t;
typedef struct gpiod_edge_event_buffer gpiod_event_buf_t;
typedef struct gpiod_edge_event        gpiod_edge_event_t;
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

enum error_num { NONE = 0,
	             OPEN_CHIP_FAILED,
				 OPEN_CHIP_ALREADY_OPENED,
				 OPEN_SETTINGS_ALREADY_CREATED,
				 OPEN_SETTINGS_FAILED_TO_CREATE,
				 CLOSE_SETTINGS_NOT_CREATED,
				 CLOSE_REQUEST_NOT_CREATED,
				 CLOSE_CHIP_NOT_OPENED,
				 SETUP_CHIP_NOT_OPENED,
				 SETUP_FAILED_TO_CREATE_REQUEST,
				 SETUP_CREATE_LINE_CONFIG_FAILED,
				 DIRECTION_FAILED_TO_ADD_SETTINGS,
				 DIRECTION_FAILED_TO_CREATE_NEW_SETTINGS,
				 DIRECTION_FAILED_TO_CREATE_REQUEST,
				 SET_ACTIVE_SETTINGS_FAILED,
				 SET_ACTIVE_REQUEST_FAILED,
				 DIRECTION_CHIP_NOT_OPENED,
				 DIRECTION_ADD_SETTINGS,
				 DIRECTION_CREATE_SETTINGS,
				 DIRECTION_CREATE_CONFIG,
				 DIRECTION_NOT_SET,
				 DIRECTION_CREATE_REQUEST,
				 SET_VALUE_NO_SETTINGS,
				 SET_VALUE_DIRECTION,
				 SET_VALUE_CREATE_REQUEST,
				 GET_VALUE_CREATE_REQUEST,
				 GET_DIRECTION_SETTINGS,
				 GET_ACTIVE_STATE_SETTINGS,
				 WAIT_FOR_EDGE_REQUEST_FAILED,
				 WAIT_FOR_EDGE_DETECTION_EVENT_FAILED,
				 SET_EDGE_DETECTION_SETTINGS_FAILED,
				 SET_EDGE_DETECTION_REQUEST_FAILED,
				 INIT_EDGE_DETECTION_REQUEST_FAILED,
				 INIT_EDGE_DETECTION_BUFFER_REQUEST_FAILED,
				 GET_EVENT_BUFFER_SIZE_FAILURE,
				 GET_EDGE_DETECTION_REQUEST_FAILED,
				 GET_EVENT_LINE_REQUEST_FAILED
			  };

class GpiodClass
{
	public:
		GpiodClass(void);
		gpio_lines_t *lines;
		unsigned     numLines;
		gpiod_event_buf_t  *buffer;
		gpiod_edge_event_t *event;
	public: // Helpers operating on linesArray[]
		unsigned get_LineNum(unsigned lineId, int *error);
		unsigned get_LineNum(string pinName, int *error);
		unsigned get_LineId(unsigned lineNum, int *error);
		unsigned get_LineId(string lineName,   int *error);
		string get_LineName(unsigned lineId, int *error);
		string get_ChipPath(unsigned lineId,  int *error);
		string get_ChipPath(string pinName,  int *error);
		string get_PinName(unsigned lineId,   int *error);
		string get_Consumer(unsigned lineId,  int *error);
		string get_Consumer(string lineName,  int *error);
		int    set_Consumer(unsigned lineId, string consumer);
		~GpiodClass(void);
	public:
		int openLine(unsigned lineId);
		int closeLine(unsigned lineId);
		int setLineDirection(unsigned lineId, unsigned direction, unsigned output);
		int setLineValue(unsigned lineId, unsigned output);
		int setLineActiveState(unsigned lineId, bool state);
		int setLineEdgeDetection(unsigned lineId, gpiod_line_edge edge);
		int setLineBias(unsigned lineId, enum gpiod_line_bias bias);
		int setLineDrive(unsigned lineId, enum gpiod_line_drive drive);
		int setLineDebounceUS(unsigned lineId, unsigned debounceUS);
		int freeLineSettings(unsigned lineId); // gpiod_line_settings_free(struct gpiod_line_settings *settings);
		int initLineEdgeEvents(unsigned lineId, enum gpiod_line_edge edge);
		int waitForLineEdgeEvent(unsigned lineId);
		int getLineEventData(unsigned lineId, unsigned n);
		int getLineValue(unsigned lineId);
		int getLineActiveState(unsigned lineId, bool *activeLow);
		int getLineDirection(unsigned lineId, unsigned *direction);
		int findLineByName(char *chipPath, char *lineName, unsigned *lineNum);
	private:
		int      commonSetup(unsigned lineId);
		unsigned getAllChipPaths(char ***pathsPtr);
		void     printErrors(unsigned errorNum, unsigned lineId);
		gpiod_chip_t     *chip;
		gpiod_settings_t *settings;
		gpiod_request_t  *request;
		gpiod_config_t   *line_cfg;
		gpiod_req_cfg_t  *req_cfg;
		char             *consumer;
		unsigned         offset;
};

#endif /* GPIOD_CLASS_H_ */

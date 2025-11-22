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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "gpiod_class.h"

GpiodClass::GpiodClass(void)
{
	chip = NULL;
	settings = NULL;
	request = NULL;
	line_cfg = NULL;
	req_cfg = NULL;
	consumer = NULL;
	buffer = NULL;
	event = NULL;
	lines = NULL;
	offset = 0;
	numLines = 0;
}
GpiodClass::~GpiodClass(void)
{
	for(unsigned i=0; i<numLines; i++)
		closeLine(i);
}
int GpiodClass::openLine(unsigned lineId)
{
	char *chip_path;
	unsigned errorNum = NONE;

	chip_path = (char *)lines[lineId].chipPath.c_str();
	chip = lines[lineId].chip;
	if(chip !=  NULL)
	{
		errorNum = OPEN_CHIP_ALREADY_OPENED;
		printErrors(errorNum, lineId);
		return -1;
	}
	chip = gpiod_chip_open(chip_path);
	if(chip == NULL)
	{
		errorNum = OPEN_CHIP_FAILED;
		printErrors(errorNum, lineId);
		gpiod_chip_close(chip);
		return -1;
	}
	else
	{
		lines[lineId].chip = chip;
	}
	settings = lines[lineId].settings;
	if(settings != NULL)
	{
		errorNum = OPEN_SETTINGS_ALREADY_CREATED;
		printErrors(errorNum, lineId);
		gpiod_chip_close(chip);
		return -1;
	}
	else
	{
		settings = gpiod_line_settings_new();
		if(settings == NULL)
		{
			errorNum = OPEN_SETTINGS_FAILED_TO_CREATE;
			printErrors(errorNum, lineId);
			return -1;
		}
		lines[lineId].settings = settings;
	}
	return 0;
}
int GpiodClass::closeLine(unsigned lineId)
{
	settings = lines[lineId].settings;
	request = lines[lineId].request;
	chip =  lines[lineId].chip;
#ifdef DEBUG
	unsigned errorNum = NONE;
	if(settings == NULL)
	{
		errorNum = CLOSE_SETTINGS_NOT_CREATED;
		printErrors(errorNum, lineId);
	}
	else
	{
		gpiod_line_settings_free(settings);
		lines[lineId].settings = NULL;
	}
	if(request == NULL)
	{
		errorNum = CLOSE_REQUEST_NOT_CREATED;
		printErrors(errorNum, lineId);
	}
	else
	{
		gpiod_line_request_release(request);
		lines[lineId].request = NULL;
	}
	if(chip == NULL)
	{
		errorNum = CLOSE_CHIP_NOT_OPENED;
		printErrors(errorNum, lineId);
		return -1;
	}
	else
	{
		gpiod_chip_close(chip);
		lines[lineId].chip = NULL;
	}
#else
	if(settings != NULL)
	{
		gpiod_line_settings_free(settings);
		lines[lineId].settings = NULL;
	}
	if(request != NULL)
	{
		gpiod_line_request_release(request);
		lines[lineId].request = NULL;
	}
	if(chip == NULL)
		return -1;
	else
	{
		gpiod_chip_close(chip);
		lines[lineId].chip = NULL;
	}
#endif
	return 0;
}
int GpiodClass::freeLineSettings(unsigned lineId)
{
	settings = lines[lineId].settings;
	if(settings == NULL)
		return -1;
	gpiod_line_settings_free(settings);
	return 0;
}
int GpiodClass::setLineBias(unsigned lineId, enum gpiod_line_bias bias)
{
    unsigned   errorNum = NONE;
    int ret;
    errorNum = commonSetup(lineId);
	settings = lines[lineId].settings;
    gpiod_line_settings_set_bias(settings, bias);
	offset = lines[lineId].lineNum;
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if(ret)
	{
		errorNum = SET_ACTIVE_SETTINGS_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if(request == NULL)
	{
		errorNum = SET_ACTIVE_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	lines[lineId].settings = settings;
	lines[lineId].request = request;
	gpiod_line_config_free(line_cfg);
	return 0;
}
int GpiodClass::setLineDrive(unsigned lineId, enum gpiod_line_drive drive)
{
    unsigned   errorNum = NONE;
    int ret;
    errorNum = commonSetup(lineId);
	settings = lines[lineId].settings;
    gpiod_line_settings_set_drive(settings, drive);
	offset = lines[lineId].lineNum;
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if(ret)
	{
		errorNum = SET_ACTIVE_SETTINGS_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if(request == NULL)
	{
		errorNum = SET_ACTIVE_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	lines[lineId].settings = settings;
	lines[lineId].request = request;
	gpiod_line_config_free(line_cfg);
	return 0;
}
int GpiodClass::setLineDebounceUS(unsigned lineId, unsigned debounceUS)
{
    unsigned   errorNum = NONE;
    int ret;
    errorNum = commonSetup(lineId);
	settings = lines[lineId].settings;
    gpiod_line_settings_set_debounce_period_us(settings, debounceUS);
	offset = lines[lineId].lineNum;
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if(ret)
	{
		errorNum = SET_ACTIVE_SETTINGS_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if(request == NULL)
	{
		errorNum = SET_ACTIVE_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	lines[lineId].settings = settings;
	lines[lineId].request = request;
	gpiod_line_config_free(line_cfg);
	return 0;
}
int GpiodClass::setLineDirection(unsigned lineId, unsigned direction, unsigned output)
{
    unsigned   errorNum = NONE;
    int ret;
    errorNum = commonSetup(lineId);
	settings = lines[lineId].settings;
	if(settings) // Change of direction free previous
	{
		gpiod_line_settings_free(settings);
		settings = NULL;
	}
	if(settings == NULL)
	{   // Saved settings, no new
		settings = gpiod_line_settings_new();
		if(settings == NULL)
		{
			errorNum = DIRECTION_FAILED_TO_CREATE_NEW_SETTINGS;
			printErrors(errorNum, lineId);
			return -1;
		}
	}
    if(output == HIGH)
		gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_ACTIVE);
	else
		gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
	if(direction == OUTPUT)
		gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
	else
		gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
	offset = lines[lineId].lineNum;
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if(ret)
	{
		errorNum = DIRECTION_FAILED_TO_ADD_SETTINGS;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if(request == NULL)
	{
		errorNum = DIRECTION_FAILED_TO_CREATE_REQUEST;
		printErrors(errorNum, lineId);
		return -1;
	}
	lines[lineId].settings = settings;
	lines[lineId].request = request;
	gpiod_line_config_free(line_cfg);
	return 0;
}
int  GpiodClass::setLineValue(unsigned lineId, unsigned output)
{
	enum gpiod_line_value value;
	unsigned errorNum = NONE;
	offset = lines[lineId].lineNum;
	settings = lines[lineId].settings;
	if(settings == NULL)
	{
		errorNum = SET_VALUE_NO_SETTINGS;
		return -1;
	}
	if(settings->direction != GPIOD_LINE_DIRECTION_OUTPUT)
	{
		errorNum = SET_VALUE_DIRECTION;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = lines[lineId].request;
	if(request == NULL)
	{
		errorNum = SET_VALUE_CREATE_REQUEST;
		printErrors(errorNum, lineId);
		return -1;
	}
	if(output)
		value = GPIOD_LINE_VALUE_ACTIVE;
	else
		value = GPIOD_LINE_VALUE_INACTIVE;
	gpiod_line_request_set_value(request, offset, value);
	return 0;
}
int  GpiodClass::setLineActiveState(unsigned lineId, bool state)
{
    unsigned   errorNum = NONE;
    int ret;
    errorNum = commonSetup(lineId);
	settings = lines[lineId].settings;
    gpiod_line_settings_set_active_low(settings, state);
	offset = lines[lineId].lineNum;
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if(ret)
	{
		errorNum = SET_ACTIVE_SETTINGS_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if(request == NULL)
	{
		errorNum = SET_ACTIVE_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	lines[lineId].settings = settings;
	lines[lineId].request = request;
	gpiod_line_config_free(line_cfg);
	return 0;
}
int  GpiodClass::setLineEdgeDetection(unsigned lineId, gpiod_line_edge edge)
{
    unsigned   errorNum = NONE;
    int ret;
    errorNum = commonSetup(lineId);
	settings = lines[lineId].settings;
    gpiod_line_settings_set_edge_detection(settings, edge);
	offset = lines[lineId].lineNum;
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if(ret)
	{
		errorNum = SET_EDGE_DETECTION_SETTINGS_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if(request == NULL)
	{
		errorNum = SET_EDGE_DETECTION_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	lines[lineId].settings = settings;
	lines[lineId].request = request;
	gpiod_line_config_free(line_cfg);
	return 0;
}
int GpiodClass::waitForLineEdgeEvent(unsigned lineId)
{
	unsigned errorNum = NONE;
	int ret;
	gpiod_request_t	*request;
	request = lines[lineId].request;
	if(request == NULL)
	{
		errorNum = WAIT_FOR_EDGE_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	ret = gpiod_line_request_read_edge_events(request, buffer, EVENT_BUFFER_SIZE);
	if((ret == -1) || (ret >= EVENT_BUFFER_SIZE))
	{
		errorNum = WAIT_FOR_EDGE_DETECTION_EVENT_FAILED;
		printErrors(errorNum, lineId);
	}
	return ret;
}
int GpiodClass::initLineEdgeEvents(unsigned lineId, enum gpiod_line_edge edge)
{
	unsigned errorNum = NONE;
	int err = setLineEdgeDetection(lineId, edge);
	if(err < 0)
	{
		errorNum = INIT_EDGE_DETECTION_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	buffer = gpiod_edge_event_buffer_new(EVENT_BUFFER_SIZE);
	if(buffer == NULL)
	{
		errorNum = INIT_EDGE_DETECTION_BUFFER_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	return 0;
}
int GpiodClass::getLineEventData(unsigned lineId, unsigned n)
{
	unsigned errorNum = NONE;
	event = gpiod_edge_event_buffer_get_event(buffer, n);
	offset = lines[lineId].lineNum;
	if(n >= EVENT_BUFFER_SIZE)
	{
		errorNum = GET_EVENT_BUFFER_SIZE_FAILURE;
		printErrors(errorNum, lineId);
		return -1;
	}
	if(event == NULL)
	{
		errorNum = GET_EDGE_DETECTION_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	if(event->line_offset != offset)
	{
		errorNum = GET_EVENT_LINE_REQUEST_FAILED;
		printErrors(errorNum, lineId);
		return -1;
	}
	return 0;
}
int GpiodClass::getLineValue(unsigned lineId)
{
	/* Example configuration - customize to suit your situation. */
	struct gpiod_line_request *request;
	enum gpiod_line_value value;
	unsigned offset, errorNum = NONE;

	request = lines[lineId].request;
	if(request == NULL)
	{
		errorNum = GET_VALUE_CREATE_REQUEST;
		printErrors(errorNum, lineId);
		return -1;
	}
	offset = lines[lineId].lineNum;
	value = gpiod_line_request_get_value(request, offset);
	if(value == GPIOD_LINE_VALUE_ACTIVE)
		return HIGH;
	return LOW;
}
int GpiodClass::getLineActiveState(unsigned lineId, bool *activeLow)
{
	unsigned errorNum = NONE;
	settings = lines[lineId].settings;
	if(settings == NULL)
	{
		errorNum = GET_ACTIVE_STATE_SETTINGS;
		printErrors(errorNum, lineId);
		return -1;
	}
	*activeLow = settings->active_low;
	return 0;
}
int GpiodClass::getLineDirection(unsigned lineId, unsigned *direction)
{
	unsigned errorNum = NONE;
	settings = lines[lineId].settings;
	if(settings == NULL)
	{
		errorNum = GET_DIRECTION_SETTINGS;
		printErrors(errorNum, lineId);
		return -1;
	}
	*direction = settings->direction;
	return 0;
}
int GpiodClass::findLineByName(char *chipPath, char *lineName, unsigned *lineNum)
{
	struct gpiod_chip_info *cinfo;
	int i, num_chips, _offset;
	char **chip_paths;
	num_chips = getAllChipPaths(&chip_paths);
	for (i = 0; i < num_chips; i++)
	{
		chip = gpiod_chip_open(chip_paths[i]);
		if (!chip)
			continue;
		_offset = gpiod_chip_get_line_offset_from_name(chip, lineName);
		if(_offset == -1)
			goto close_chip;
		cinfo = gpiod_chip_get_info(chip);
		if(!cinfo)
			goto close_chip;
#ifdef DEBUG
		fprintf(stderr, "%s: %s %d\n", lineName, gpiod_chip_info_get_name(cinfo), _offset);
#endif
		*lineNum = _offset;
		strcpy(chipPath, chip_paths[i]);
		return 0;
close_chip:
		gpiod_chip_close(chip);
	}
#ifdef DEBUG
	fprintf(stderr, "line '%s' not found\n", lineName);
#endif
	return -1;
}
#include <dirent.h>
static int chipDirFilter(const struct dirent *entry)
{
	struct stat sb;
	int ret = 0;
	char *path;
	if(asprintf(&path, "/dev/%s", entry->d_name) < 0)
		return 0;

	if((lstat(path, &sb) == 0) && (!S_ISLNK(sb.st_mode)) && gpiod_is_gpiochip_device(path))
		ret = 1;
	free(path);
	return ret;
}
unsigned GpiodClass::getAllChipPaths(char ***pathsPtr)
{
	//static int all_chip_paths(char ***paths_ptr)
	int i, j, num_chips;
	unsigned ret = 0;
	struct dirent **entries;
	char **paths;

	num_chips = scandir("/dev/", &entries, chipDirFilter, versionsort);
	if (num_chips < 0)
		return 0;
	paths = (char **)calloc(num_chips, sizeof(*paths));
	if (!paths)
		return 0;
	for(i = 0; i < num_chips; i++)
	{
		if(asprintf(&paths[i], "/dev/%s", entries[i]->d_name) < 0)
		{
			for (j = 0; j < i; j++)
				free(paths[j]);
			free(paths);
			return 0;
		}
	}
	*pathsPtr = paths;
	ret = num_chips;
	for(i = 0; i < num_chips; i++)
		free(entries[i]);
	free(entries);
	return ret;
}
unsigned GpiodClass::get_LineNum(unsigned lineId, int *error)
{
	*error = 0;
	if(lineId > numLines)
		*error = ERROR;
	return lines[lineId].lineNum;
}
unsigned GpiodClass::get_LineNum(string lineName, int *error)
{
	*error = 0;
	for(unsigned i=0; i<numLines; i++)
	{   // Search for a match in our lines array
		if(lineName == lines[i].lineName)
			return lines[i].lineNum;
	}
	*error = ERROR;
	return numLines;
}
unsigned GpiodClass::get_LineId(unsigned lineNum, int *error)
{
	*error = 0;
	for(unsigned i=0; i<numLines; i++)
	{   // Search for a match in our lines array
		if(lineNum == lines[i].lineNum)
			return i;
	}
	*error = ERROR;
	return numLines;
}
unsigned GpiodClass::get_LineId(string lineName, int *error)
{
	*error = 0;
	for(unsigned i=0; i<numLines; i++)
	{   // Search for a match in our lines array
		if(lineName == lines[i].lineName)
			return i;
	}
	*error = ERROR;
	return numLines;
}

string GpiodClass::get_LineName(unsigned lineId, int *error)
{
	*error = 0;
	if(lineId < numLines)
		return lines[lineId].lineName;
	*error = ERROR;
	return "";
}
string GpiodClass::get_ChipPath(unsigned lineId, int *error)
{
	*error = 0;
	if(lineId < numLines)
		return  lines[lineId].chipPath;
	*error = ERROR;
	return "";
}
string GpiodClass::get_ChipPath(string lineName, int *error)
{
	*error = 0;
	for(unsigned i=0; i<numLines; i++)
	{
		if(lineName == lines[i].lineName)
			return lines[i].chipPath;
	}
	return "";
}
string GpiodClass::get_Consumer(unsigned lineId,  int *error)
{
	*error = 0;
	if(lineId < numLines)
		return  lines[lineId].consumer;
	*error = ERROR;
	return "";
}
string GpiodClass::get_Consumer(string lineName,  int *error)
{
	*error = NO_ERROR;
	for(unsigned i=0; i<numLines; i++)
	{
		if(lineName == lines[i].lineName)
			return lines[i].consumer;
	}
	*error = ERROR;
	return "";
}
int GpiodClass::set_Consumer(unsigned lineId, string consumer)
{
	if(lineId >= numLines)
		return ERROR;
	lines[lineId].consumer = consumer;
	return 0;
}
void GpiodClass::printErrors(unsigned errorNum, unsigned lineId)
{
	unsigned lineNum = lines[lineId].lineNum;
	const char *chipPath = lines[lineId].chipPath.c_str();
	switch(errorNum)
	{
	case NONE:
		return;
	case OPEN_CHIP_FAILED:
		fprintf(stderr, "ERROR: Open chip (%s) failed\n", chipPath);
		break;
	case OPEN_CHIP_ALREADY_OPENED:
		fprintf(stderr, "ERROR: Open Chip (%s) is already opened\n", chipPath);
		break;
	case OPEN_SETTINGS_ALREADY_CREATED:
		fprintf(stderr, "ERROR: Open settings already created, line #%d\n", lineNum);
		break;
	case OPEN_SETTINGS_FAILED_TO_CREATE:
		fprintf(stderr, "ERROR: Open, settings failed to create, line #%d\n", lineNum);
		break;
	case CLOSE_SETTINGS_NOT_CREATED:
		fprintf(stderr, "ERROR: Close, settings not created, line #%d\n", lineNum);
		break;
	case CLOSE_REQUEST_NOT_CREATED:
		fprintf(stderr, "ERROR: Close, line request not created, line #%d\n", lineNum);
		break;
	case CLOSE_CHIP_NOT_OPENED:
		fprintf(stderr, "ERROR: Close chip (%s), not opened\n", chipPath);
		break;
	case SETUP_CHIP_NOT_OPENED:
		fprintf(stderr, "ERROR: Setup chip (%s), not opened\n", chipPath);
		break;
	case SETUP_FAILED_TO_CREATE_REQUEST:
		fprintf(stderr, "ERROR: Setup failed to create request, line #%d\n", lineNum);
		break;
	case SETUP_CREATE_LINE_CONFIG_FAILED:
		fprintf(stderr, "ERROR: Setup failed to create a line configuration, line #%d\n", lineNum);
		break;
	case DIRECTION_FAILED_TO_CREATE_NEW_SETTINGS:
		fprintf(stderr, "ERROR: Failed to create new settings structure, line #%d\n", lineNum);
		break;
	case DIRECTION_FAILED_TO_ADD_SETTINGS:
		fprintf(stderr, "ERROR: Direction failed to add settings, line #%d\n", lineNum);
		break;
	case SET_VALUE_NO_SETTINGS:
		fprintf(stderr, "ERROR: Settings not created\n");
		break;
	case SET_VALUE_DIRECTION:
		fprintf(stderr, "ERROR: Direction not set to OUTPUT\n");
		break;
	case SET_VALUE_CREATE_REQUEST:
		fprintf(stderr, "ERROR: Line requested failed: %s, line #%d\n", strerror(errno), lineNum);
		break;
	case SET_ACTIVE_SETTINGS_FAILED:
		fprintf(stderr, "ERROR: Set active settings failed. line #%d\n", lineNum);
		break;
	case SET_ACTIVE_REQUEST_FAILED:
		fprintf(stderr, "ERROR: Set active line request failed: %s, line #%d\n", strerror(errno), lineNum);
		break;
	case SET_EDGE_DETECTION_SETTINGS_FAILED:
		fprintf(stderr, "ERROR: Set edge detection settings failed, line #%d\n", lineNum);
		break;
	case SET_EDGE_DETECTION_REQUEST_FAILED:
		fprintf(stderr, "ERROR: Set edge detection line request failed, line #%d\n", lineNum);
		break;
	case GET_VALUE_CREATE_REQUEST:
		fprintf(stderr, "ERROR: Get value line request was not created, line #%d\n", lineNum);
		break;
	case WAIT_FOR_EDGE_REQUEST_FAILED:
		fprintf(stderr, "ERROR: Wait for edge request object not created, line #%d\n", lineNum);
		break;
	case WAIT_FOR_EDGE_DETECTION_EVENT_FAILED:
		fprintf(stderr, "ERROR: Wait for edge detection events failed: %s, line #%d\n", strerror(errno), lineNum);
		break;
	case INIT_EDGE_DETECTION_REQUEST_FAILED:
		fprintf(stderr, "ERROR: Edge detection failed request: %s, line #%d\n", strerror(errno), lineNum);
		break;
	case INIT_EDGE_DETECTION_BUFFER_REQUEST_FAILED:
		printErrors(errorNum, lineId);
		fprintf(stderr, "ERROR: Edge detection failed to create event buffer: %s, line #%d\n", strerror(errno), lineNum);
		break;
	case GET_EVENT_BUFFER_SIZE_FAILURE:;
		fprintf(stderr, "ERROR: Get line event data buffer size failure: %s, line #%d\n", strerror(errno), lineNum);
		break;
	case GET_EDGE_DETECTION_REQUEST_FAILED:
		fprintf(stderr, "ERROR: Edge event object not created\n");
		break;
	case GET_EVENT_LINE_REQUEST_FAILED:
		fprintf(stderr, "ERROR: Edge event wrong line\n");
		break;
	case GET_ACTIVE_STATE_SETTINGS:
	case GET_DIRECTION_SETTINGS:
	default:
		break;

	}
}
int GpiodClass::commonSetup(unsigned lineId)
{
	// Set private variables
    unsigned   errorNum = NONE;

	offset = lines[lineId].lineNum;
    chip = lines[lineId].chip;
	if(chip == NULL)
	{   // Created via open
		errorNum = SETUP_CHIP_NOT_OPENED;
		printErrors(errorNum, lineId);
		return ERROR;
	}
	request = lines[lineId].request;
	if(request)
	{
		gpiod_line_request_release(request);
		request = NULL;
	}
	line_cfg = gpiod_line_config_new();
	if(line_cfg == NULL)
	{
		errorNum = SETUP_CREATE_LINE_CONFIG_FAILED;
		printErrors(errorNum, lineId);
		return ERROR;
	}
	req_cfg = gpiod_request_config_new();
	if(req_cfg == NULL)
	{
		errorNum = SETUP_FAILED_TO_CREATE_REQUEST;
		printErrors(errorNum, lineId);
		return ERROR;
	}
	consumer = (char *)lines[lineId].consumer.c_str();
	gpiod_request_config_set_consumer(req_cfg, consumer);
	return NO_ERROR;
}

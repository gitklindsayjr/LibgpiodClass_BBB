/*
 * gpiod_functions.h
 *
 *  Created on: Oct 12, 2025
 *      Author: pops
 */

#ifndef GPIOD_STRUCTS_H_
#define GPIOD_STRUCTS_H_

#include <linux/gpio.h>
#ifdef NATIVE
	#include <gpiod.h>
#else
	#include "gpiod.h"
#endif

#define LINES_MAX (GPIO_V2_LINES_MAX)
#define EVENT_BUFFER_MAX_CAPACITY (GPIO_V2_LINES_MAX * 16)

struct gpiod_chip {
	int fd;
	char *path;
};

struct gpiod_chip_info {
	size_t num_lines;
	char name[GPIO_MAX_NAME_SIZE + 1];
	char label[GPIO_MAX_NAME_SIZE + 1];
};
struct gpiod_line_info {
	unsigned int offset;
	char name[GPIO_MAX_NAME_SIZE + 1];
	bool used;
	char consumer[GPIO_MAX_NAME_SIZE + 1];
	enum gpiod_line_direction direction;
	bool active_low;
	enum gpiod_line_bias bias;
	enum gpiod_line_drive drive;
	enum gpiod_line_edge edge;
	enum gpiod_line_clock event_clock;
	bool debounced;
	unsigned long debounce_period_us;
};
struct gpiod_line_settings {
	enum gpiod_line_direction direction;
	enum gpiod_line_edge edge_detection;
	enum gpiod_line_drive drive;
	enum gpiod_line_bias bias;
	bool active_low;
	enum gpiod_line_clock event_clock;
	long debounce_period_us;
	enum gpiod_line_value output_value;
};
struct gpiod_line_request {
	char *chip_name;
	unsigned int offsets[GPIO_V2_LINES_MAX];
	size_t num_lines;
	int fd;
};
struct settings_node {
	struct settings_node *prev;
	struct settings_node *next;
	struct gpiod_line_settings *settings;
	unsigned int refcnt;
};

struct per_line_config {
	unsigned int offset;
	struct settings_node *node;
};

struct gpiod_line_config {
	struct per_line_config line_configs[LINES_MAX];
	size_t num_configs;
	enum gpiod_line_value output_values[LINES_MAX];
	size_t num_output_values;
	struct settings_node *sref_list;
};
struct gpiod_info_event {
	enum gpiod_info_event_type event_type;
	uint64_t timestamp;
	struct gpiod_line_info *info;
};
struct gpiod_edge_event {
	enum gpiod_edge_event_type event_type;
	uint64_t timestamp;
	unsigned int line_offset;
	unsigned long global_seqno;
	unsigned long line_seqno;
};

struct gpiod_edge_event_buffer {
	size_t capacity;
	size_t num_events;
	struct gpiod_edge_event *events;
	struct gpio_v2_line_event *event_data;
};

#endif /* GPIOD_STRUCTS_H_ */

/*
 * Copyright (C) 2018 Paul Kocialkowski <paul.kocialkowski@bootlin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CEDRUS_FRAME_TEST_H_
#define _CEDRUS_FRAME_TEST_H_

#include <stdbool.h>
#include <linux/types.h>
#include <linux/v4l2-controls.h>

/*
 * Macros
 */

#define ALIGN(x, y) ((x + (y - 1)) & ~(y - 1))
#define DIV_ROUND_UP(x, y) ((x + (y - 1)) / y)

/*
 * Structures
 */

/* Cedrus Frame Test */

struct config {
	char *video_path;
	char *media_path;
	char *drm_path;
	char *drm_driver;
	unsigned int crtc_id;
	unsigned int plane_id;

	char *preset_name;
	char *slices_path;
	char *slices_filename_format;

	unsigned int buffers_count;
	unsigned int fps;
	bool quiet;
	bool interactive;
	bool loop;
};

/* Presets */

enum frame_type {
	DUMP_FRAME_MPEG2,
};

union controls {
	struct {
		struct v4l2_ctrl_mpeg2_slice_header header;
	} mpeg2;
};

struct frame {
	unsigned int index;
	union controls frame;
};

struct preset {
	char *name;
	char *description;
	char *license;
	char *attribution;

	unsigned int width;
	unsigned int height;

	enum frame_type type;
	struct frame *frames;
	unsigned int frames_count;
};

/* V4L2 */

struct video_buffer {
	void *source_data;
	unsigned int source_size;
	void *destination_data[2];
	unsigned int destination_size[2];
	int export_fds[2];
	int request_fd;
};


/* DRM */

struct gem_buffer {
	void *data;
	unsigned int size;
	unsigned int handles[4];
	unsigned int pitches[4];
	unsigned int offsets[4];

	unsigned int framebuffer_id;
};

struct display_properties_ids {
	uint32_t connector_crtc_id;
	uint32_t crtc_mode_id;
	uint32_t crtc_active;
	uint32_t plane_fb_id;
	uint32_t plane_crtc_id;
	uint32_t plane_src_x;
	uint32_t plane_src_y;
	uint32_t plane_src_w;
	uint32_t plane_src_h;
	uint32_t plane_crtc_x;
	uint32_t plane_crtc_y;
	uint32_t plane_crtc_w;
	uint32_t plane_crtc_h;
};

struct display_setup {
	unsigned int connector_id;
	unsigned int encoder_id;
	unsigned int crtc_id;
	unsigned int plane_id;

	unsigned int width;
	unsigned int height;
	unsigned int x;
	unsigned int y;
	unsigned int scaled_width;
	unsigned int scaled_height;

	unsigned int buffers_count;
	bool use_dmabuf;

	struct display_properties_ids properties_ids;
};

/*
 * Functions
 */

/* Presets */

void presets_usage(void);
struct preset *preset_find(char *name);
int frame_header_fill(struct v4l2_ctrl_mpeg2_slice_header *header, struct preset *preset, unsigned int index, unsigned int slice_size);
int frame_gop_next(unsigned int *index);
int frame_gop_dequeue(void);
int frame_gop_queue(unsigned int index);
int frame_gop_schedule(struct preset *preset, unsigned int index);

/* V4L2 */

int video_engine_start(int video_fd, int media_fd, unsigned int width, unsigned int height, struct video_buffer **buffers, unsigned int buffers_count);
int video_engine_stop(int video_fd, struct video_buffer *buffers, unsigned int buffers_count);
int video_engine_decode(int video_fd, unsigned int index, struct v4l2_ctrl_mpeg2_slice_header *header, void *source_data, unsigned int source_size, struct video_buffer *buffers);

/* DRM */

int display_engine_start(int drm_fd, unsigned int width, unsigned int height, struct video_buffer *video_buffers, unsigned int count, struct gem_buffer **buffers, struct display_setup *setup);
int display_engine_stop(int drm_fd, struct gem_buffer *buffers, struct display_setup *setup);
int display_engine_show(int drm_fd, unsigned int index, struct video_buffer *video_buffers, struct gem_buffer *buffers, struct display_setup *setup);

#endif

/*
 * Copyright (c) 2024 tinyVision.ai Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT tinyvision_example

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(example, CONFIG_VIDEO_LOG_LEVEL);

struct example_data {
	struct video_format fmt;
};

struct video_format_cap fmts[] = {
	{
    		.pixelformat = VIDEO_PIX_FMT_YUYV,
		.width_min = 1280,
		.width_max = 1280,
		.height_min = 1024,
		.height_max = 1024,
	},
	{0},
};

static int example_set_fmt(const struct device *dev, struct video_format *fmt)
{
	struct example_data *data = dev->data;
	size_t ind;
	int ret;

	ret = video_format_caps_index(fmts, fmt, &ind);
	if (ret != 0) {
		LOG_ERR("Format %08x %ux%u unsupported", fmt->pixelformat, fmt->width, fmt->height);
		return -EINVAL;
	}

	data->fmt = *fmt;

	return 0;
}

static int example_get_fmt(const struct device *dev, struct video_format *fmt)
{
	struct example_data *data = dev->data;

	*fmt = data->fmt;

	return 0;
}

static int example_get_caps(const struct device *dev, struct video_caps *caps)
{
	caps->format_caps = fmts;

	return 0;
}

static int example_get_frmival(const struct device *dev, struct video_frmival *frmival)
{
	frmival->numerator = 1;
	frmival->denominator = 60;

	return 0;
}

static int example_enum_frmival(const struct device *dev, struct video_frmival_enum *fie)
{
	if (fie->index > 0) {
		return 1;
	}

	fie->type = VIDEO_FRMIVAL_TYPE_DISCRETE;
	fie->discrete.numerator = 1;
	fie->discrete.denominator = 60;
	fie->index++;

	return 0;
}

static int example_set_stream(const struct device *dev, bool on, enum video_buf_type)
{
	return 0;
}

static const struct video_driver_api example_driver_api = {
	.set_format = example_set_fmt,
	.get_format = example_get_fmt,
	.get_caps = example_get_caps,
	.get_frmival = example_get_frmival,
	.enum_frmival = example_enum_frmival,
	.set_stream = example_set_stream,
};

static int example_init(const struct device *dev)
{
	struct video_format fmt = {
		.pixelformat = VIDEO_PIX_FMT_YUYV,
		.width = 1280,
		.height = 1024,
		.pitch = fmt.width * 2,
    	};

	return example_set_fmt(dev, &fmt);
}

#define EXAMPLE_INIT(n)                                                                            \
	struct example_data example_data_##n;                                                      \
	DEVICE_DT_INST_DEFINE(n, &example_init, NULL, &example_data_##n, NULL, POST_KERNEL,        \
			      CONFIG_VIDEO_INIT_PRIORITY, &example_driver_api);

DT_INST_FOREACH_STATUS_OKAY(EXAMPLE_INIT)

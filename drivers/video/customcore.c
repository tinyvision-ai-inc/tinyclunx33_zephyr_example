/*
 * Copyright (c) 2024 tinyVision.ai Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT tinyvision_customcore

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(customcore, CONFIG_VIDEO_LOG_LEVEL);

struct customcore_data {
	struct video_format fmt;
};

struct video_format_cap fmts[] = {
	{.pixelformat = VIDEO_PIX_FMT_YUYV,
	 .width_min = 1280,
	 .width_max = 1280,
	 .height_min = 1024,
	 .height_max = 1024}, /* SXGA */
	{0},
};

static int customcore_set_fmt(const struct device *dev, enum video_endpoint_id ep,
			  struct video_format *fmt)
{
	struct customcore_data *data = dev->data;
	int i;

	if (ep != VIDEO_EP_OUT && ep != VIDEO_EP_ALL) {
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(fmts); ++i) {
		if (fmt->pixelformat == fmts[i].pixelformat &&
		    IN_RANGE(fmt->width, fmts[i].width_min, fmts[i].width_max) &&
		    IN_RANGE(fmt->height, fmts[i].height_min, fmts[i].width_max)) {
			break;
		}
	}
	if (i == ARRAY_SIZE(fmts)) {
		LOG_ERR("Format %08x %ux%u unsupported", fmt->pixelformat, fmt->width, fmt->height);
		return -EINVAL;
	}

	data->fmt = *fmt;
	return 0;
}

static int customcore_get_fmt(const struct device *dev, enum video_endpoint_id ep,
			  struct video_format *fmt)
{
	struct customcore_data *data = dev->data;

	*fmt = data->fmt;
	return 0;
}

static int customcore_get_caps(const struct device *dev, enum video_endpoint_id ep,
			   struct video_caps *caps)
{
	caps->format_caps = fmts;
	return 0;
}

static int customcore_get_frmival(const struct device *dev, enum video_endpoint_id ep,
			       struct video_frmival *frmival)
{
	if (ep != VIDEO_EP_OUT && ep != VIDEO_EP_ALL) {
		return -EINVAL;
	}

	frmival->numerator = 1;
	frmival->denominator = 60;
	return 0;
}

static int customcore_enum_frmival(const struct device *dev, enum video_endpoint_id ep,
				struct video_frmival_enum *fie)
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

static int customcore_stream_start(const struct device *dev)
{
	return 0;
}

static int customcore_stream_stop(const struct device *dev)
{
	return 0;
}

static const struct video_driver_api customcore_driver_api = {
	.set_format = customcore_set_fmt,
	.get_format = customcore_get_fmt,
	.get_caps = customcore_get_caps,
	.get_frmival = customcore_get_frmival,
	.enum_frmival = customcore_enum_frmival,
	.stream_start = customcore_stream_start,
	.stream_stop = customcore_stream_stop,
};

static int customcore_init(const struct device *dev)
{
	struct video_format fmt;

	fmt.pixelformat = VIDEO_PIX_FMT_YUYV;
	fmt.width = 1280;
	fmt.height = 1024;
	fmt.pitch = fmt.width * 2;
	return customcore_set_fmt(dev, VIDEO_EP_OUT, &fmt);
}

#define CUSTOMCORE_INIT(n)                                                                       \
	struct customcore_data customcore_data_##n;                                            \
	DEVICE_DT_INST_DEFINE(n, &customcore_init, NULL, &customcore_data_##n, NULL,           \
			      POST_KERNEL, CONFIG_VIDEO_INIT_PRIORITY, &customcore_driver_api);

DT_INST_FOREACH_STATUS_OKAY(CUSTOMCORE_INIT)

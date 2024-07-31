/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/i2c.h>

void main(void)
{
	printf("hello\n");
	k_sleep(K_SECONDS(1));

	const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));
	if (video_dev == NULL || !device_is_ready(video_dev)) {
		printf("Could not get I2C cam device\n");
		return;
	}
	printk("Video device: %s\n", video_dev->name);
}

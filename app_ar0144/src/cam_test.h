#ifndef CAM_TEST
#define CAM_TEST
#include <stdio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/gpio.h>
#include <inttypes.h>
#include <zephyr/sys/util.h>
#include <stdbool.h>


int ar0144Init(void);

#endif 
#include <stdio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/video.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app2, LOG_LEVEL_DBG);

#define STACKSIZE 500
#define PRIORITY 5

const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));

static int gain_control(const struct shell *shell, size_t argc, char **argv) {
    if (argc != 2) {
        shell_print(shell, "Usage: camera gain <increase|decrease> <scale>");
        return -1;
    }

    int scale_value = 1;
    scale_value = atoi(argv[1]);
    if (scale_value == 0) {
        shell_print(shell, "scale_value Usage: camera gain <increase|decrease> <scale>");
        return -1;
    }

	if (video_dev == NULL || !device_is_ready(video_dev)) {
		printf("Could not get I2C cam device\n");
		return -1;
	}
	printf("Video device: %s\n", video_dev->name);

	shell_print(shell, "first value: %s\n", argv[0]);

    uint8_t value;
    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN,&value)){
            shell_print(shell, "Unable to set gain value = %d.\n", value);
            return -1;
    }

    if (strcmp(argv[0], "increase") == 0) {
        value += scale_value;

		if(video_set_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN,&value)){
			 shell_print(shell, "Unable to set gain value = %d.\n", value);
			 return -1;
		}
        shell_print(shell, "Gain increased to %d.\n", value);
    } else if (strcmp(argv[0], "decrease") == 0) {
        value -= scale_value;
		if(video_set_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN,&value)){
			 shell_print(shell, "Unable to set gain value = %d.\n", value);
			 return -1;
		}
        shell_print(shell, "Gain decreased to %d.\n", value);
    } else {
        shell_print(shell, "Invalid argument: %s. Use 'increase' or 'decrease'.", argv[0]);
        return -1;
    }
    return 0;
}

static int exposure_control(const struct shell *shell, size_t argc, char **argv) {
    shell_print(shell, "Usage: camera exposure <increase|decrease>");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_gain,
    SHELL_CMD_ARG(increase, NULL, "Increase camera gain", gain_control, 2, 0),
    SHELL_CMD_ARG(decrease, NULL, "Decrease camera gain", gain_control, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_exposure,
    SHELL_CMD_ARG(increase, NULL, "Increase camera exposure", exposure_control, 2, 0),
    SHELL_CMD_ARG(decrease, NULL, "Decrease camera exposure", exposure_control, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(cam_ctrls, 
			SHELL_CMD(gain, &sub_gain, "increase or decrease camera gain", NULL),
			SHELL_CMD(exposur, &sub_exposure, "increase or decrease camera exposure", NULL),
			SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(imx219, &cam_ctrls, "camera controls", NULL);
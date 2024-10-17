#include <stdio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/video.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
LOG_MODULE_REGISTER(app2, LOG_LEVEL_DBG);

const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));

static int gain_control(const struct shell *shell, size_t argc, char **argv) {

    int scale_value = 1;
    uint8_t value, min, max, dflt;

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN,&value)){
        shell_print(shell, "Unable to get current gain value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN_DEF,&dflt)){
        shell_print(shell, "Unable to get default gain value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN_MIN,&min)){
        shell_print(shell, "Unable to get min gain value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_GAIN_MAX,&max)){
        shell_print(shell, "Unable to get max gain value");
        return -1;
    }

    if (strcmp(argv[0], "current_value") == 0) {
        shell_print(shell, "Current gain = %d", value);
        shell_print(shell, "Default gain = %d", dflt);
        shell_print(shell, "Minimum gain = %d", min);
        shell_print(shell, "Maximum gain = %d", max);
        return 0;
    }

    scale_value = atoi(argv[1]);
    // if (scale_value == 0) {
    //     shell_print(shell, "scale_value Usage: gain <increase|decrease> <scale>");
    //     return -1;
    // }

    if(scale_value < min || scale_value > max){
        shell_print(shell, "Input value is out of range");
        return -1;
    }

    if (strcmp(argv[0], "increase") == 0) {
        value += scale_value;
    } else if (strcmp(argv[0], "decrease") == 0) {
        value -= scale_value;
    } else if (strcmp(argv[0], "custom") == 0) {
        value = scale_value;
    } else if (strcmp(argv[0], "reset") == 0) {
        value = -1;
    } else {
        shell_print(shell, "Invalid argument: %s. Use 'increase' or 'decrease'", argv[0]);
        return -1;
    }

    if (video_set_ctrl(video_dev, VIDEO_CID_CAMERA_GAIN, &value)) {
        shell_print(shell, "Unable to set gain value = %d", value);
        return -1;
    }

    shell_print(shell, "Gain %s to %d.", (strcmp(argv[0], "increase") == 0) ? "increased" : "decreased", value);
    return 0;
}

static int exposure_control(const struct shell *shell, size_t argc, char **argv) {
    int scale_value = 5;
    uint16_t value, min, max, dflt;

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_EXPOSURE,&value)){
        shell_print(shell, "Unable to get current exposure value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_EXPOSURE_DEF,&dflt)){
        shell_print(shell, "Unable to get default exposure value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_EXPOSURE_MIN,&min)){
        shell_print(shell, "Unable to get min exposure value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_EXPOSURE_MAX,&max)){
        shell_print(shell, "Unable to get max exposure value");
        return -1;
    }

    if (strcmp(argv[0], "current_value") == 0) {
        shell_print(shell, "Current exposure = %d", value);
        shell_print(shell, "Default exposure = %d", dflt);
        shell_print(shell, "Minimum exposure = %d", min);
        shell_print(shell, "Maximum exposure = %d", max);
        return 0;
    }

    scale_value = atoi(argv[1]);
    // if (scale_value == 0) {
    //     shell_print(shell, "scale_value Usage: exposure <increase|decrease> <scale>");
    //     return -1;
    // }

    if(scale_value > max){
        shell_print(shell, "Input value is out of range");
        return -1;
    }

    if (strcmp(argv[0], "increase") == 0) {
        value += scale_value;
        //shell_print(shell, "scale_value =  %d\tvalue = %d", scale_value, value);
    } else if (strcmp(argv[0], "custom") == 0) {
        value = scale_value;
    } else if (strcmp(argv[0], "reset") == 0) {
        value = -1;
    } else if (strcmp(argv[0], "decrease") == 0) {
        value -= scale_value;
    } else {
        shell_print(shell, "Invalid argument: %s. Use 'increase' or 'decrease'", argv[0]);
        return -1;
    }

    if (video_set_ctrl(video_dev, VIDEO_CID_CAMERA_EXPOSURE, &value)) {
        shell_print(shell, "Unable to set exposure value = %d", value);
        return -1;
    }

    shell_print(shell, "exposure %s to %d", (strcmp(argv[0], "increase") == 0) ? "increased" : "decreased", sys_be16_to_cpu(value));
    return 0;
}

static int digital_gain_control(const struct shell *shell, size_t argc, char **argv) {

    int scale_value = 1;
    uint16_t value, min, max, dflt;

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_DIGITAL_GAIN,&value)){
        shell_print(shell, "Unable to get current gain value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_DIGITAL_GAIN_DEF,&dflt)){
        shell_print(shell, "Unable to get default gain value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_DIGITAL_GAIN_MIN,&min)){
        shell_print(shell, "Unable to get min gain value");
        return -1;
    }

    if(video_get_ctrl(video_dev,VIDEO_CID_CAMERA_DIGITAL_GAIN_MAX,&max)){
        shell_print(shell, "Unable to get max gain value");
        return -1;
    }

    if (strcmp(argv[0], "current_value") == 0) {
        shell_print(shell, "Current gain = %d", value);
        shell_print(shell, "Default gain = %d", dflt);
        shell_print(shell, "Minimum gain = %d", min);
        shell_print(shell, "Maximum gain = %d", max);
        return 0;
    }
    shell_print(shell, "Current gain = %d", value);
    scale_value = atoi(argv[1]);
    // if (scale_value == 0) {
    //     shell_print(shell, "scale_value Usage: gain <increase|decrease> <scale>");
    //     return -1;
    // }

    if (strcmp(argv[0], "increase") == 0) {
        value += scale_value;
    } else if (strcmp(argv[0], "decrease") == 0) {
        value -= scale_value;
    } else if (strcmp(argv[0], "custom") == 0) {
        value = scale_value;
    } else if (strcmp(argv[0], "reset") == 0) {
        value = -1;
    } else {
        shell_print(shell, "Invalid argument: %s. Use 'increase' or 'decrease'", argv[0]);
        return -1;
    }

    if(value < min || value > max){
        shell_print(shell, "Input value is out of range");
        return -1;
    }
    shell_print(shell, "change gain = %d", value);
    if (video_set_ctrl(video_dev, VIDEO_CID_CAMERA_DIGITAL_GAIN, &value)) {
        shell_print(shell, "Unable to set gain value = %d", value);
        return -1;
    }

    shell_print(shell, "Gain digital %s to %d.", (strcmp(argv[0], "increase") == 0) ? "increased" : "decreased", sys_be16_to_cpu(value));
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_gain,
    SHELL_CMD_ARG(increase, NULL, "Usage: gain increase <scale>", gain_control, 2, 0),
    SHELL_CMD_ARG(decrease, NULL, "Usage: gain decrease <scale>", gain_control, 2, 0),
    SHELL_CMD_ARG(custom, NULL, "Usage: gain custom <scale>", gain_control, 2, 0),
    SHELL_CMD_ARG(reset, NULL, "Usage: gain reset", gain_control, 1, 0),
    SHELL_CMD_ARG(current_value, NULL, "Usage: gain current_value", gain_control, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_exposure,
    SHELL_CMD_ARG(increase, NULL, "Usage: exposur increase", exposure_control, 2, 0),
    SHELL_CMD_ARG(decrease, NULL, "Usage: exposur decrease", exposure_control, 2, 0),
    SHELL_CMD_ARG(custom, NULL, "Usage: exposur custom <scale>", exposure_control, 2, 0),
    SHELL_CMD_ARG(reset, NULL, "Usage: exposur reset", exposure_control, 1, 0),
    SHELL_CMD_ARG(current_value, NULL, "Usage: exposure current_value", exposure_control, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_digital_gain,
    SHELL_CMD_ARG(increase, NULL, "Usage: digital gain increase", digital_gain_control, 2, 0),
    SHELL_CMD_ARG(decrease, NULL, "Usage: digital gain decrease", digital_gain_control, 2, 0),
    SHELL_CMD_ARG(custom, NULL, "Usage: digital gain custom <scale>", digital_gain_control, 2, 0),
    SHELL_CMD_ARG(reset, NULL, "Usage: digital gain reset", digital_gain_control, 1, 0),
    SHELL_CMD_ARG(current_value, NULL, "Usage: digital gain current_value", digital_gain_control, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(cam_ctrls, 
			SHELL_CMD(gain, &sub_gain, "increase or decrease camera gain", NULL),
			SHELL_CMD(exposure, &sub_exposure, "increase or decrease camera exposure", NULL),
            SHELL_CMD(digital_gain, &sub_digital_gain, "increase or decrease camera digital gain", NULL),
			SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(imx219, &cam_ctrls, "camera controls", NULL);
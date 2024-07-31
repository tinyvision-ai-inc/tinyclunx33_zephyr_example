#include <stdio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/video.h>
const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));

static void cam_init() {

    //const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));
    if (video_dev == NULL)
    {
        printf("Could not get IMX219 device\n");
        return;
    }
    
	printf("Video device: %s\n", video_dev->name);

    int ret = device_init(video_dev);
    if(ret != 0){
        printf("device not found or failed to init\n");
        return;
    }

    printf("Device initialized successfully ......\n");
}

static void cam_start() {
    
    struct video_format fmt;
    struct video_caps caps;
    int i = 0;

    //const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));
    if (video_dev == NULL || !device_is_ready(video_dev))
    {
        printf("Could not get I2C cam device\n");
        return;
    }
	printf("Video device: %s\n", video_dev->name);

    /* Get capabilities */
	if (video_get_caps(video_dev, VIDEO_EP_OUT, &caps)) {
		printf("Unable to retrieve video capabilities\n");
		return;
	}

	printf("Capabilities:\n");
	while (caps.format_caps[i].pixelformat) {
		const struct video_format_cap *fcap = &caps.format_caps[i];
		/* fourcc to string */
		printf("  %c%c%c%c width [%u; %u; %u] height [%u; %u; %u]\n",
		       (char)fcap->pixelformat, (char)(fcap->pixelformat >> 8),
		       (char)(fcap->pixelformat >> 16), (char)(fcap->pixelformat >> 24),
		       fcap->width_min, fcap->width_max, fcap->width_step, fcap->height_min,
		       fcap->height_max, fcap->height_step);
		i++;
	}

    if (video_get_format(video_dev, VIDEO_EP_OUT, &fmt)) {
		printf("Unable to retrieve video format\n");
		return;
	}

	printf("Default format: %c%c%c%c %ux%u\n", (char)fmt.pixelformat,
	       (char)(fmt.pixelformat >> 8), (char)(fmt.pixelformat >> 16),
	       (char)(fmt.pixelformat >> 24), fmt.width, fmt.height);


    /* Start video capture */
	if (video_stream_start(video_dev)) {
		printf("Unable to start capture (interface)\n");
		return;
	}
    printf("Streaming......\n");
}

static void cam_stop() {

    //const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));
    if (video_dev == NULL || !device_is_ready(video_dev))
    {
        printf("Could not get I2C cam device\n");
        return;
    }
	printf("Video device: %s\n", video_dev->name);

	if (video_stream_stop(video_dev)) {
		printf("Unable to start capture (interface)\n");
		return;
	}
    printf("stoping camera......\n");
}

SHELL_STATIC_SUBCMD_SET_CREATE(cam_sub,
                                SHELL_CMD(cam_init, NULL, "Initialize camera",
                                         &cam_init),
                                SHELL_CMD(cam_start, NULL, "start camera streaming",
                                         &cam_start),
                                SHELL_CMD(cam_stop, NULL, "stop camera streaming",
                                         &cam_stop),      
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(imx219, &cam_sub, "camera command", NULL);
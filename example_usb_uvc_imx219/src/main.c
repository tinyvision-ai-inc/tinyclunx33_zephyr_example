#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/video.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

static const struct device *uvc0_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));

// static void cam_start(void)
// {

// 	struct video_format fmt;
// 	struct video_caps caps;
// 	int i = 0;

// 	// const struct device *video_dev = DEVICE_DT_GET(DT_NODELABEL(imx219));
// 	if (video_dev == NULL || !device_is_ready(video_dev)) {
// 		printf("Could not get I2C cam device\n");
// 		return;
// 	}
// 	printf("Video device: %s\n", video_dev->name);

// 	/* Get capabilities */
// 	if (video_get_caps(video_dev, VIDEO_EP_OUT, &caps)) {
// 		printf("Unable to retrieve video capabilities\n");
// 		return;
// 	}

// 	printf("Capabilities:\n");
// 	while (caps.format_caps[i].pixelformat) {
// 		const struct video_format_cap *fcap = &caps.format_caps[i];
// 		/* fourcc to string */
// 		printf("  %c%c%c%c width [%u; %u; %u] height [%u; %u; %u]\n",
// 		       (char)fcap->pixelformat, (char)(fcap->pixelformat >> 8),
// 		       (char)(fcap->pixelformat >> 16), (char)(fcap->pixelformat >> 24),
// 		       fcap->width_min, fcap->width_max, fcap->width_step, fcap->height_min,
// 		       fcap->height_max, fcap->height_step);
// 		i++;
// 	}

// 	if (video_get_format(video_dev, VIDEO_EP_OUT, &fmt)) {
// 		printf("Unable to retrieve video format\n");
// 		return;
// 	}

// 	printf("Default format: %c%c%c%c %ux%u\n", (char)fmt.pixelformat,
// 	       (char)(fmt.pixelformat >> 8), (char)(fmt.pixelformat >> 16),
// 	       (char)(fmt.pixelformat >> 24), fmt.width, fmt.height);

// 	/* Start video capture */
// 	if (video_stream_start(video_dev)) {
// 		printf("Unable to start capture (interface)\n");
// 		return;
// 	}
// 	printf("Streaming......\n");
// }

int main(void)
{
	struct video_buffer vbuf = {0};
	struct video_format fmt = {0};
	int err;

	/* Get the video format once it is selected by the host */
	while (true) {
		err = video_get_format(uvc0_dev, VIDEO_EP_IN, &fmt);
		if (err == 0) {
			break;
		}
		if (err != -EAGAIN) {
			LOG_ERR("failed to get the video format");
			return err;
		}

		k_sleep(K_MSEC(10));
	}

	/* The buffer address is the FIFO endpoint */
	vbuf.buffer = (void *)DT_REG_ADDR_BY_NAME(DT_NODELABEL(uvcmanager), fifo);
	vbuf.size = vbuf.bytesused = fmt.pitch * fmt.height;
	vbuf.flags = VIDEO_BUF_EOF;

	LOG_DBG("frame %ux%u, %u bytes, addr %p", fmt.width, fmt.height, fmt.pitch * fmt.height, vbuf.buffer);

	/* Only the first buffer needs to be enqueued when using the UVC Manager */
	err = video_enqueue(uvc0_dev, VIDEO_EP_IN, &vbuf);
	if (err) {
		LOG_ERR("failed to enqueue the video buffer");
		return err;
	}

	k_sleep(K_FOREVER);
	return 0;
}

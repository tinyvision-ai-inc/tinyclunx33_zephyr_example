#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/drivers/video.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

#define UVC0 DEVICE_DT_GET(DT_NODELABEL(uvc0))
#define UDC0 DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0))
#define SPEEDS (USB_BOS_SPEED_SUPERSPEED_GEN1 | USB_BOS_SPEED_HIGHSPEED | USB_BOS_SPEED_FULLSPEED)

static const struct usb_bos_capability_superspeed_usb bos_cap_ss = {
	.bLength = sizeof(struct usb_bos_capability_superspeed_usb),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_SUPERSPEED_USB,
	.bmAttributes = 0,
	.wSpeedsSupported = sys_cpu_to_le16(SPEEDS),
	.bFunctionnalSupport = 1,
	.bU1DevExitLat = 10,
	.wU2DevExitLat = sys_cpu_to_le16(1023),
};
USBD_DESC_BOS_DEFINE(usbd_bos_cap_ss, sizeof(bos_cap_ss), &bos_cap_ss);

static const struct usb_bos_capability_lpm bos_cap_lpm = {
	.bLength = sizeof(struct usb_bos_capability_lpm),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
	.bmAttributes = USB_BOS_ATTRIBUTES_LPM,
};
USBD_DESC_BOS_DEFINE(usbd_bos_cap_lpm, sizeof(bos_cap_lpm), &bos_cap_lpm);

USBD_DEVICE_DEFINE(usbd, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0x1209, 0x0001);
USBD_CONFIGURATION_DEFINE(usbd_config, USB_SCD_SELF_POWERED, 100);
USBD_DESC_MANUFACTURER_DEFINE(usbd_manufacturer, "tinyVision.ai");
USBD_DESC_PRODUCT_DEFINE(usbd_product, "tinyCLUNX33");
USBD_DESC_LANG_DEFINE(usbd_lang);

int main(void)
{
	int ret = 0;

	ret |= usbd_add_descriptor(&usbd, &usbd_lang);
	ret |= usbd_add_descriptor(&usbd, &usbd_manufacturer);
	ret |= usbd_add_descriptor(&usbd, &usbd_product);
	ret |= usbd_add_descriptor(&usbd, &usbd_bos_cap_lpm);
	ret |= usbd_add_descriptor(&usbd, &usbd_bos_cap_ss);
	ret |= usbd_add_configuration(&usbd, USBD_SPEED_SS, &usbd_config);
	__ASSERT_NO_MSG(ret == 0);

	usbd_device_set_code_triple(&usbd, USBD_SPEED_SS, USB_BCC_MISCELLANEOUS, 0x02, 0x01);

	ret |= usbd_register_all_classes(&usbd, USBD_SPEED_SS, 1);
	__ASSERT_NO_MSG(ret == 0);

	ret |= usbd_init(&usbd);
	__ASSERT_NO_MSG(ret == 0);

	ret |= usbd_enable(&usbd);
	__ASSERT_NO_MSG(ret == 0);

	ret = video_stream_start(UVC0);
	__ASSERT_NO_MSG(ret == 0);

	while (true) {
		usb23_irq_handler(UDC0);
		k_sleep(K_MSEC(10));
	}

	return 0;
}

/*
 * Send a single video frame whenever hitting the "video frame" command on the shell.
 * This permits to open a video feed with a webcam viewer tool such as VLC, and manually
 * step through every frame.
 *
 * If wanting a continuous video feed instead, repeat this function in loop, eventually
 * only the enqueue()/dequeue() part for efficiency, and possibly with several buffers
 * as done in the video Zephyr samples.
 */
static int cmd_video_frame(const struct shell *sh, size_t argc, char **argv)
{
	struct video_buffer *vbuf;
	struct video_format fmt = {0};
	static uint8_t byte = 0x00;
	int ret;

	ret = video_get_format(UVC0, VIDEO_EP_IN, &fmt);
	if (ret != 0) {
		shell_error(sh, "could not allocate the video buffer");
		return ret;
	}

	shell_print(sh, "Preparing a %ux%u frame, %u bytes",
		    fmt.pitch, fmt.height, fmt.pitch * fmt.height);

	/* If this gets stuck, you might need to increase CONFIG_VIDEO_BUFFER_POOL_SZ_MAX */
	vbuf = video_buffer_alloc(fmt.pitch * fmt.height);
	if (vbuf == NULL) {
		shell_error(sh, "could not allocate the video buffer");
		return -ENOMEM;
	}

	memset(vbuf->buffer, byte, vbuf->size);
	vbuf->bytesused = vbuf->size;
	vbuf->flags = VIDEO_BUF_EOF;

	ret = video_enqueue(UVC0, VIDEO_EP_IN, vbuf);
	if (ret != 0) {
		shell_error(sh, "could not enqueue video buffer");
		goto end;
	}

	ret = video_dequeue(UVC0, VIDEO_EP_IN, &vbuf, K_FOREVER);
	if (ret != 0) {
		shell_error(sh, "could not dequeue video buffer");
		goto end;
	}

	byte ^= 0xff;
end:
	video_buffer_release(vbuf);
	return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_video,
                               SHELL_CMD(frame, NULL, "submit a frame of specified color",
                                         &cmd_video_frame),
                               SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(video, &sub_video, "Video controls", NULL);

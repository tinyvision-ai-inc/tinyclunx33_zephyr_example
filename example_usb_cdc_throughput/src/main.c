#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/net/buf.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/class/usb_cdc_raw.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* Static pool of buffer with no data in it */
NET_BUF_POOL_DEFINE(app_buf_pool, 2, 0, sizeof(struct udc_buf_info), NULL);

static const struct device *cdc0_dev = DEVICE_DT_GET(DT_NODELABEL(cdc0));

uint8_t usb23_dma_buf[2048];

static void app_worker(struct k_work *work)
{
	struct net_buf *buf;
	int err;

	/* This will wait until _write_callback frees some more buffers */
	buf = net_buf_alloc_with_data(&app_buf_pool, usb23_dma_buf, sizeof(usb23_dma_buf), K_NO_WAIT);
	if (buf == NULL) {
		return;
	}

	LOG_INF("Sending buffer, addr %p, size %u", usb23_dma_buf, sizeof(usb23_dma_buf));
	err = cdc_raw_write(cdc0_dev, buf, false);
	if (err) {
		LOG_DBG("Could not send buffer");
	}

	/* Enqueue a new buffer until there is no more buffer available */
	k_work_submit(work);
}
K_WORK_DEFINE(app_work, app_worker);

static int _write_callback(const struct device *dev, struct net_buf *buf, int err)
{
	struct udc_buf_info *bi = udc_get_buf_info(buf);

	if (err) {
		LOG_ERR("USB write error %d", err);
		return err;
	}
	LOG_DBG("%s: buf=%p data=%p len=%d size=%d zlp=%d", __func__,
		buf, buf->data, buf->len, buf->size, bi->zlp);

	/* Freeing here will unblock the memory pool */
	net_buf_unref(buf);

	/* Submit another processing for these events */
	k_work_submit(&app_work);

	return 0;
}

int main(void)
{
	/* Data that can be easily recognized */
	for (size_t i = 0; i < sizeof(usb23_dma_buf); i++) {
		usb23_dma_buf[i] = i;
	}

	/* Hook the callback called used once a write is completed */
	cdc_raw_set_write_callback(cdc0_dev, &_write_callback);

	/* Wait that a serial console connects */
	while (!cdc_raw_is_ready(cdc0_dev)) {
		k_sleep(K_MSEC(100));
	}

	LOG_INF("Starting the bulk throughput test...");
	k_work_submit(&app_work);

	k_sleep(K_FOREVER);
	return 0;
}

// An I2C MUX exists at either 0x70 or 0x71 depending on the slot used
#define I2C_MUX_ADDR 0x70

static void sel_cam(uint8_t slot, uint8_t cam_id) {
	printf("Selecting camera %d\n\r", cam_id);
	uint8_t val = 0x4 + cam_id;
	i2c_write(I2C_MUX_ADDR + slot, 0, &val, 1, 1);
}

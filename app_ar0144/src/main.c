#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

const bool I2C_WRITE = true;
const bool I2C_READ = false;
const uint8_t I2C_MUX_ADDR = 0x71;  
const uint8_t I2C_GPIO_EXPANDER_ADDR = 0x21;  
const uint8_t I2C_AR0144_ADDR = 0x18;


struct i2c_write_cmd {
    uint8_t addr;
    uint16_t reg;
    uint16_t value;
};

const struct i2c_write_cmd init_sequence[] = {
    {I2C_MUX_ADDR, 0x0, 0x2},           // Select I2C port 0/1
    {I2C_GPIO_EXPANDER_ADDR, 0x6, 0x0},  // Set GPIO expander port 0 to output
    {I2C_GPIO_EXPANDER_ADDR, 0x7, 0x0},  // Set GPIO expander port 1 to output
    {I2C_GPIO_EXPANDER_ADDR, 0x3, 0x0},  // Set output to low for expander port 1
    {I2C_GPIO_EXPANDER_ADDR, 0x2, 0x7},  // Power enable, hold in reset
};

static int cam_write(const struct device *i2c_dev, uint16_t reg, uint16_t value)
{
        int ret;
        uint8_t buf[4];

        /* Setup register address bytes */
        buf[0] = reg >> 8;
        buf[1] = reg & 0xFF;
        buf[2] = value >> 8;
        buf[3] = value & 0xFF;

        ret = i2c_write(i2c_dev, buf, sizeof(buf), I2C_AR0144_ADDR);
        if (ret != 0) {
                LOG_ERR("Camera I2C write failed, reg: 0x%04x, value: 0x%04x, ret: %d",
                        reg, value, ret);
                return ret;
        }

        return 0;
}

struct cam_write_cmd {
    uint16_t addr;
    uint16_t value;
};

const struct cam_write_cmd cam_init_sequence[] = {
    // PLL Settings
    {0x302A, 0x0004},  // VT_PIX_CLK_DIV = 4
    {0x302C, 0x0002},  // VT_SYS_CLK_DIV = 2
    {0x302E, 0x0002},  // PRE_PLL_CLK_DIV = 2
    {0x3030, 0x0032},  // PLL_MULTIPLIER = 50
    {0x3036, 0x0008},  // OP_PIX_CLK_DIV = 8
    {0x3038, 0x0001},  // OP_SYS_CLK_DIV = 1
    {0x30B0, 0x0000},  // DIGITAL_TEST, bits 0x4010 = 0
    {0x31B0, 0x004A},  // FRAME_PREAMBLE = 74
    {0x31B2, 0x0029},  // LINE_PREAMBLE = 41
    {0x31B4, 0x3C56},  // MIPI_TIMING_0 = 15446
    {0x31B6, 0x3218},  // MIPI_TIMING_1 = 12824
    {0x31B8, 0x40CB},  // MIPI_TIMING_2 = 16587
    {0x31BA, 0x028A},  // MIPI_TIMING_3 = 650
    {0x31BC, 0x8008},  // MIPI_TIMING_4 = 32776
    {0x3354, 0x002A},  // MIPI_CNTRL = 42

    // Timing Settings
    {0x31AE, 0x0201},  // SERIAL_FORMAT = 513
    {0x3002, 0x0000},  // Y_ADDR_START = 0
    {0x3004, 0x0004},  // X_ADDR_START = 4
    {0x3006, 0x031F},  // Y_ADDR_END = 799
    {0x3008, 0x0503},  // X_ADDR_END = 1283
    {0x300A, 0x0341},  // FRAME_LENGTH_LINES = 833
    {0x300C, 0x05DC},  // LINE_LENGTH_PCK = 1500
    {0x3012, 0x0032},  // COARSE_INTEGRATION_TIME = 50
    {0x31AC, 0x0808},  // DATA_FORMAT_BITS = 2056
    {0x306E, 0x9010},  // DATAPATH_SELECT = 36880
    {0x30A2, 0x0001},  // X_ODD_INC = 1
    {0x30A6, 0x0001},  // Y_ODD_INC = 1
    {0x3082, 0x0003},  // OPERATION_MODE_CTRL = 3
    {0x3084, 0x0003},  // OPERATION_MODE_CTRL_CB = 3
    {0x308C, 0x0028},  // Y_ADDR_START_CB = 40
    {0x308A, 0x0004},  // X_ADDR_START_CB = 4
    {0x3090, 0x02F7},  // Y_ADDR_END_CB = 759
    {0x308E, 0x0503},  // X_ADDR_END_CB = 1283
    {0x30AA, 0x0349},  // FRAME_LENGTH_LINES_CB = 841
    {0x303E, 0x05D0},  // LINE_LENGTH_PCK_CB = 1488
    {0x3016, 0x0033},  // COARSE_INTEGRATION_TIME_CB = 51
    {0x30AE, 0x0001},  // X_ODD_INC_CB = 1
    {0x30A8, 0x0001},  // Y_ODD_INC_CB = 1
    {0x3040, 0x0000},  // READ_MODE = 0
    {0x31D0, 0x0000},  // COMPANDING = 0
    {0x301A, 0x005C}   // RESET_REGISTER = 92
};

const struct cam_write_cmd cam_start[] = {
    //{0x301A, 0x30dc},   // RESET_REGISTER
    {0x301a, 0x205c}   //start
};

int main(void)
{
	LOG_INF("Hello World");
    const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

    if (!device_is_ready(i2c_dev)) {
        LOG_ERR("I2C device not ready");
        return -ENODEV;
    }
    uint8_t buf4[4] = {0,};
    uint8_t buf2[2] = {0,};
    for (size_t i = 0; i < ARRAY_SIZE(init_sequence); i++) {
        uint16_t value = init_sequence[i].value;
        buf2[0] = init_sequence[i].reg;
        buf2[1] = value & 0xFF;
        i2c_write(i2c_dev, buf2, sizeof(buf2), init_sequence[i].addr);
    }
    k_sleep(K_MSEC(100)); // Sleep for 10ms to let power stabilize
    buf2[0] = 0x2;
    buf2[1] = 0xc7;
    i2c_write(i2c_dev, buf2, sizeof(buf2), I2C_GPIO_EXPANDER_ADDR); // Release reset

    k_sleep(K_MSEC(100)); // Sleep for a bit to let the camera stabilize
    for (size_t i = 0; i < ARRAY_SIZE(cam_init_sequence); i++) {
        cam_write(i2c_dev, cam_init_sequence[i].addr, cam_init_sequence[i].value);
    }

    for (size_t i = 0; i < ARRAY_SIZE(cam_start); i++) {
        cam_write(i2c_dev, cam_start[i].addr, cam_start[i].value);
    }

    
	return 0;
}

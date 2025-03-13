// #include <stdio.h>
// #include <zephyr/shell/shell.h>
// #include <zephyr/devicetree.h>
// #include <zephyr/device.h>
// #include <zephyr/logging/log.h>
// #include <zephyr/kernel.h>
// #include <zephyr/drivers/i2c.h>
// #include <zephyr/drivers/video.h>
// #include <zephyr/drivers/gpio.h>
// #include <inttypes.h>
// #include <zephyr/sys/util.h>
// #include <stdbool.h>

#include "cam_test.h"

LOG_MODULE_REGISTER(app_cam2_init, LOG_LEVEL_DBG);

#define SW1_NODE        DT_ALIAS(sw1)
#if DT_NODE_HAS_STATUS(SW1_NODE, okay)
static struct gpio_dt_spec sw1_spec = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
#endif

#define SW2_NODE        DT_ALIAS(sw2)
#if DT_NODE_HAS_STATUS(SW2_NODE, okay)
static struct gpio_dt_spec sw2_spec = GPIO_DT_SPEC_GET(SW2_NODE, gpios);
#endif

#define LED0_NODE DT_ALIAS(led0)
#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#endif

#define LED1_NODE DT_ALIAS(led1)
#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
#endif

#define CAM0_FAILED (1<<0)
#define CAM1_FAILED (1<<1)

uint8_t flag = 0;

static void reg_32b_read_2(uint32_t addr, uint32_t *val)
{
	*val = *((volatile unsigned int *)addr);
}

struct camData{
    bool test_result;
    uint8_t fps;
    uint16_t rows, columns;
    uint32_t r_value, gr_value, gb_value, b_value, frame_count;

};  

static bool buttonFn(const struct gpio_dt_spec *sw_spec){
	int ret;

	if (!gpio_is_ready_dt(sw_spec)) {
		printf("%s is not ready\n", sw_spec->port->name);
		return false;
	}

    ret = gpio_pin_configure_dt(sw_spec, GPIO_INPUT);
    if (ret < 0) {
        printf("Error: Failed to configure button pin\n");
        return false;
    }

	int val = gpio_pin_get_dt(sw_spec);
	if (val > 0) {  
		
		while (gpio_pin_get_dt(sw_spec) > 0) {
			k_msleep(10);
		}
		return true;
	}
   return false;
}

static int ledControl(const struct gpio_dt_spec *spec, int value){
	int ret;

	if (!gpio_is_ready_dt(spec)) {
		printf("%s is not ready\n", spec->port->name);
		return -1;
	}

    ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printf("Error: Failed to configure button pin\n");
        return -1;
    }

	ret = gpio_pin_set_dt(spec, value);
	if (ret < 0) {
		return -1;
	}
	return 0;
}

static int configurePins(const struct device *dev, uint8_t port, uint8_t config_mask){
    if (!device_is_ready(dev)) {
        printf("pi4iOe5v6416 device not ready!\n");
        return -1;
    }
	uint8_t start_pin = port * 8;  
	uint8_t end_pin = start_pin + 8; 
    for (uint8_t pin = start_pin; pin < end_pin; pin++) {
        if (config_mask & (1 << pin)) {
            int ret = gpio_pin_configure(dev, pin, GPIO_OUTPUT_INACTIVE);
            if (ret != 0) {
                printf("Failed to configure pin %d as output (error %d)\n", pin, ret);
            }
        } else {
            int ret = gpio_pin_configure(dev, pin, GPIO_OUTPUT_INACTIVE);
            if (ret != 0) {
                printf("Failed to configure pin %d as input (error %d)\n", pin, ret);
            }
        }
    }

	return 0;
}

static int setPinValues(const struct device *dev, uint8_t port, uint8_t mask){

    if (!device_is_ready(dev)) {
        printf("pi4iOe5v6416 device not ready!\n");
        return -1;
    }
	int ret;
	uint8_t start_pin = port * 8;  
	uint8_t end_pin = start_pin + 8; 

    for (int pin = start_pin; pin < end_pin; pin++) {
        int value = (mask & (1 << pin)) ? 1 : 0;
        ret = gpio_pin_set(dev, pin, value);
		if (ret < 0) {
			return -1;
		}
		k_sleep(K_MSEC(10));
    }

	return 0;
}

static int camInit(const struct device *dev){
	int err;
	err = device_init(dev);	
	if (err) {
		return err;
	}

	LOG_DBG("Camera driver initialized successfully...");
	return 0;
}

static int camStart(const struct device *dev){

	if (video_stream_start(dev)) {
		return -1;
	}
	LOG_DBG("%s : Cam streaming.", dev->name);
	return 0;
}

static int camStop(const struct device *dev){
    flag = 0;

	if (video_stream_stop(dev)) {
		return -1;
	}
	LOG_DBG("%s : Cam stopped.", dev->name);
	return 0;
}

static int checkCsr(const int *cam, struct camData *data){
    uint32_t frame_count_1, frame_count_2, 
        frame_count_addr, res_addr, res_value,
        r_addr, gr_addr, gb_addr, b_addr,
        r_value, gr_value, gb_value, b_value;

    if(*cam == 0){
        printf("inside cam 0 csr \n");
        frame_count_addr = 0xb2000010;
        res_addr = 0xb2000014;
        r_addr = 0xb200001c;
        gr_addr = 0xb2000020;
        gb_addr = 0xb2000024;
        b_addr = 0xb2000028;
    } else {
        printf("inside cam 1 csr \n");
        frame_count_addr = 0xb2100010;
        res_addr = 0xb2100014;
        r_addr = 0xb210001c;
        gr_addr = 0xb2100020;
        gb_addr = 0xb2100024;
        b_addr = 0xb2100028;
    }
    reg_32b_read_2(frame_count_addr, &frame_count_1);
    k_sleep(K_MSEC(1000));
    reg_32b_read_2(frame_count_addr, &frame_count_2);
    reg_32b_read_2(res_addr, &res_value);
    reg_32b_read_2(r_addr, &r_value);
    reg_32b_read_2(gr_addr, &gr_value);
    reg_32b_read_2(gb_addr, &gb_value);
    reg_32b_read_2(b_addr, &b_value);
    data->rows = (res_value >> 16) & 0xFFFF;
    data->columns = res_value & 0xFFFF;
    data->r_value = r_value;
    data->gr_value = gr_value;
    data->gb_value = gb_value;
    data->b_value = b_value;
    data->fps = frame_count_2-frame_count_1; 
    data->frame_count = frame_count_2;
    if(frame_count_1 < frame_count_2){
        data->test_result = true;
        return 0;
    }
    data->test_result = false;
	return -1;
}

void printRow(const char *label, const char *val1, const char *val2) {
    printf("| %-12s | %-28s | %-28s |\n", label, val1, val2);
}

void printTable(struct camData *dev1, struct camData *dev2) {

    printf("| %-12s | %-28s | %-28s |\n", "Data", "CAM1", "CAM2");
    printf("|--------------|------------------------------|------------------------------|\n");
    // printRow("Result", dev1->test_result ? "Pass" : "Fail", dev2->test_result ? "Pass" : "Fail");
    printf("| %-12s | %-28s | %-28s |\n", "Result", dev1->test_result ? "Pass" : "Fail", dev2->test_result ? "Pass" : "Fail");
    // printf("| %-12s | %-28d | %-28d |\n", "FPS", dev1->fps, dev2->fps);       // Uncomment this for fps
    printf("| %-12s | %-28u | %-28u |\n", "Frame Count ", dev1->frame_count, dev2->frame_count);
    printf("| %-12s | %-28d | %-28d |\n", "Rows", dev1->rows, dev2->rows);
    printf("| %-12s | %-28d | %-28d |\n", "Columns", dev1->columns, dev2->columns);
    printf("| %-12s | %-28u | %-28u |\n", "Red ", dev1->r_value, dev2->r_value);
    printf("| %-12s | %-28u | %-28u |\n", "Green - Red", dev1->gr_value, dev2->gr_value);
    printf("| %-12s | %-28u | %-28u |\n", "Green - Blue", dev1->gb_value, dev2->gb_value);
    printf("| %-12s | %-28u | %-28u |\n", "Blue", dev1->b_value, dev2->b_value);
}

void clearData(struct camData *data){
    data->test_result = false;
    data->rows = 0;
    data->columns = 0;
    data->fps = 0;
    data->r_value = 0;
    data->gr_value = 0;
    data->gb_value = 0;
    data->b_value = 0;
    data->frame_count = 0;
}
 
int ar0144Init(void)
{
    static const struct device *io_dev = DEVICE_DT_GET(DT_NODELABEL(pi4ioe5v6416));
    if (!device_is_ready(io_dev)) {
        LOG_ERR("I2C device not ready");
        return -ENODEV;
    }
    static const struct device *cam1 = DEVICE_DT_GET(DT_NODELABEL(ar0144));
	static const struct device *cam2 = DEVICE_DT_GET(DT_NODELABEL(ar0144_2));
    struct camData *cam1_data = NULL;
    struct camData *cam2_data = NULL;
    int cam1_val = 0;
    int cam2_val = 1;

    ledControl(&led0, 0);
	ledControl(&led1, 0);
    while(1){
        int ret;
        cam1_data = k_malloc(sizeof(struct camData));
        if(cam1_data == NULL){
            LOG_ERR("Failed to allocate memory.");
            break;
        }
        cam2_data = k_malloc(sizeof(struct camData));
        if(cam2_data == NULL){
            LOG_ERR("Failed to allocate memory.");
            break;
        }
        LOG_INF("Press SW1 to start the testing if camera is connected.");
        while(!buttonFn(&sw1_spec)){
			k_msleep(10);
		}
		LOG_INF("Camera Start button pressed.");

         /* Configure io expander */
		ret = configurePins(io_dev, 0, 0x00);
		if(ret < 0){
			LOG_ERR("Unable to turn on the camera.");
		}

		ret = setPinValues(io_dev, 0, 0xc7);
		if(ret < 0){
			LOG_ERR("Unable to turn on the camera.");
		}   
        
        ret = camInit(cam1); 
        if(ret  == 0){
            ret = camStart(cam1);
            if(ret == 0){
            	ret = checkCsr(&cam1_val, cam1_data);
                if(ret < 0){
                    flag |= CAM0_FAILED;
                    clearData(cam1_data);
                    LOG_ERR("No frame count for CAM 1.");
                }
            } else {
                flag |= CAM0_FAILED;
                clearData(cam1_data);
                LOG_ERR("Unable to start capture for CAM 1.");
            } 
        } else{
            flag |= CAM0_FAILED;
            clearData(cam1_data);
			LOG_ERR("Unable to initialize CAM 1.");
		}

        ret = camInit(cam2); 
        if(ret  == 0){
            ret = camStart(cam2);
            if(ret == 0){
            	ret = checkCsr(&cam2_val, cam2_data);
                if(ret < 0){
                    flag |= CAM1_FAILED;
                    clearData(cam2_data);
                    LOG_ERR("No frame count for CAM 2.");
                }
            } else {
                flag |= CAM1_FAILED;
                clearData(cam2_data);
                LOG_ERR("Unable to start capture for CAM 2.");
            } 
        } else{
            flag |= CAM1_FAILED;
            clearData(cam2_data);
			LOG_ERR("Unable to initialize CAM 2.");
		}

        
        k_msleep(100);
        if(flag & CAM0_FAILED || flag & CAM1_FAILED){
			ledControl(&led0, 1);
		} else{
			ledControl(&led1, 1);
		}
        printTable(cam1_data, cam2_data);

        LOG_INF("Camera test finished, Press SW2 to continue.");

        /* Stop button */
		while(!buttonFn(&sw2_spec)){	
			k_msleep(10);
		}
		LOG_INF("Camera Stop button pressed!");
        // k_msleep(20);

        /* Stop Capture */
		ret = camStop(cam1);
        if(ret < 0){
			LOG_ERR("Unable to stop capture for CAM 1.");
		}
        // k_msleep(20);

        ret = camStop(cam2);
        if(ret < 0){
			LOG_ERR("Unable to stop capture for CAM 2.");
		}
        // k_msleep(20);
        ret = setPinValues(io_dev, 0, 0x00);
		if(ret < 0){
			LOG_ERR("Unable to power off the camera.");
		}
        // k_msleep(20);

		/* Turn off LEDs */
		ledControl(&led0, 0);
		ledControl(&led1, 0);

        k_free(cam1_data);
        k_free(cam2_data);
        LOG_INF("Replace the camera.");
		k_msleep(50);
    }
    
	return 0;
}

// SHELL_CMD_REGISTER(dual_cam_init, NULL, "AR0144 sensor test commands", ar0144Init);

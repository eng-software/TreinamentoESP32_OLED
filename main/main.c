#include "driver/i2c_master.h"
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"

#define I2C_BUS_PORT            0
#define EXAMPLE_PIN_NUM_SDA     5
#define EXAMPLE_PIN_NUM_SCL     4

static const char *TAG = "example";

/**
 * @brief Entry point of the application.
 *
 * This function configures the I2C master mode and scans the bus for devices.
 * The bus is configured to use GPIO 5 for SDA and GPIO 4 for SCL, and the
 * clock speed is set to 100000 Hz. The scan starts from address 1 and goes
 * to address 126 (inclusive). If a device is found at an address, a message
 * is printed to the console with the address of the device.
 */
void app_main() 
{
    //------------------------------------------------
    // I2C scan
    //------------------------------------------------
    ESP_LOGI(TAG, "Initialize I2C bus");    
    i2c_master_bus_handle_t i2c_bus = NULL;    
    i2c_master_bus_config_t bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    printf("Scanning I2C bus...\n");
    for (int i = 1; i < 127; i++) 
    {
        esp_err_t err = i2c_master_probe(i2c_bus, i, -1);
        if (err == ESP_OK) 
        {
            printf("Found device at 0x%02x\n", i);                
        }                    
    }
    i2c_del_master_bus(i2c_bus);
    //---------------------
}

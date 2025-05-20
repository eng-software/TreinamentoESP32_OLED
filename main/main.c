#include "driver/i2c_master.h"
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#include "esp_lvgl_port.h"
#include "lvgl.h"

static const char *TAG = "example";

#define I2C_BUS_PORT            0
#define EXAMPLE_PIN_NUM_SDA     5
#define EXAMPLE_PIN_NUM_SCL     4

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              128
#define EXAMPLE_LCD_V_RES              64
void displayInit();

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
    
    //------------------------------------------------
    // LVGL
    //------------------------------------------------
    displayInit();
        
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(label, "Hello Espressif, Hello LVGL.");    
    lv_obj_set_width(label, EXAMPLE_LCD_H_RES);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);    
}


void displayInit()
{
    //------------------------------------------------
    // I2C Initialization
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
    
    //------------------------------------------------
    // SSD1306 Initialization
    //------------------------------------------------
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = 
    {
        .dev_addr            = 0x3C,
        .scl_speed_hz        = 400000,
        .control_phase_bytes = 1,   // According to SSD1306 datasheet
        .lcd_cmd_bits        = 8,   // According to SSD1306 datasheet
        .lcd_param_bits      = 8,   // According to SSD1306 datasheet
        .dc_bit_offset       = 6,   // According to SSD1306 datasheet
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));
    

    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = 
    {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = 
    {
        .height = EXAMPLE_LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    //------------------------------------------------

    
    //------------------------------------------------
    // LVGL Initialization
    //------------------------------------------------
    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = 
    {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = true,
        .rotation = 
        {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);
    //------------------------------------------------
}

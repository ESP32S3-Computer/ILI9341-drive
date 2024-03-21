#include "driver/spi_master.h"
#include "driver/gpio.h"

typedef struct {
    spi_device_handle_t spi_device;
    gpio_num_t dc_pin;
    gpio_num_t rst_pin;
} ili9341_config_t;

esp_err_t ili9341_sand_command(const ili9341_config_t device, const uint8_t command);
esp_err_t ili9341_sand_data(const ili9341_config_t device, const void* data, const size_t length);
void ili9341_address_set(const ili9341_config_t device, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void ili9341_init(const ili9341_config_t device);
int16_t RGB888_to_RGB565(uint8_t r,uint8_t g,uint8_t b);
void ili9341_clear(const ili9341_config_t device, uint16_t px);
void ili9341_show_frame(const ili9341_config_t device, void* frame);
void ili9341_write(const ili9341_config_t device, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, void* data, size_t len);

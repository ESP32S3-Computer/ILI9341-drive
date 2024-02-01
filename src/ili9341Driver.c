#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "ili9341Driver.h"

esp_err_t ili9341_sand_command(const ili9341_config_t device, const uint8_t command){
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 8;
    transaction.tx_buffer = &command;
    gpio_set_level(device.dc_pin, 0);
    return spi_device_polling_transmit(device.spi_device, &transaction);
}

// len: data的长度以bit为单位
esp_err_t ili9341_sand_data(const ili9341_config_t device, const void* data, const size_t length){
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = length;
    transaction.tx_buffer = data;
    gpio_set_level(device.dc_pin, 1);
    return spi_device_polling_transmit(device.spi_device, &transaction);
}

void ili9341_address_set(const ili9341_config_t device, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2){
    // 可优化 int8->int32
    uint8_t data[4];

    data[0] = x1>>8;
    data[1] = x1;
    data[2] = x2>>8;
    data[3] = x2;
    ili9341_sand_command(device, 0x2a);
    ili9341_sand_data(device, data, 32);

    data[0] = y1>>8;
    data[1] = y1;
    data[2] = y2>>8;
    data[3] = y2;
    ili9341_sand_command(device, 0x2b);
    ili9341_sand_data(device, data, 32);
}

void ili9341_init(const ili9341_config_t device){
    gpio_set_level(device.rst_pin, 1);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    gpio_set_level(device.rst_pin, 0);
    vTaskDelay(15 / portTICK_PERIOD_MS);
    gpio_set_level(device.rst_pin, 1);
    vTaskDelay(15 / portTICK_PERIOD_MS);

    uint8_t data[8];

    data[0] = 0x39;
    data[1] = 0x2C;
    data[2] = 0x00;
    data[3] = 0x34;
    data[4] = 0x02;
    ili9341_sand_command(device, 0xCB);
    ili9341_sand_data(device, data, 40);

    data[0] = 0x00;
    data[1] = 0XC1;
    data[2] = 0X30;
    ili9341_sand_command(device, 0xCF);
    ili9341_sand_data(device, data, 24);

    data[0] = 0x85;
    data[1] = 0x00;
    data[2] = 0x78;
    ili9341_sand_command(device, 0xE8);
    ili9341_sand_data(device, data, 24);

    data[0] = 0x00;
    data[1] = 0x00;
    ili9341_sand_command(device, 0xEA);
    ili9341_sand_data(device, data, 16);
 
    data[0] = 0x64;
    data[1] = 0x03;
    data[2] = 0X12;
    data[3] = 0X81;
    ili9341_sand_command(device, 0xED);
    ili9341_sand_data(device, data, 32);

    data[0] = 0x20;
    ili9341_sand_command(device, 0xF7);
    ili9341_sand_data(device, data, 8);
  
    data[0] = 0x23;
    ili9341_sand_command(device, 0xC0);    //Power control 
    ili9341_sand_data(device, data, 8);
 
    data[0] = 0x10;
    ili9341_sand_command(device, 0xC1);    //Power control 
    ili9341_sand_data(device, data, 8);

    data[0] = 0x3e;
    data[1] = 0x28;
    ili9341_sand_command(device, 0xC5);    //VCM control 
    ili9341_sand_data(device, data, 16);
 
    data[0] = 0x86;
    ili9341_sand_command(device, 0xC7);    //VCM control2 
    ili9341_sand_data(device, data, 8);
 
    data[0] = 0x20;
    ili9341_sand_command(device, 0x36);    // Memory Access Control 
    ili9341_sand_data(device, data, 8);

    data[0] = 0x55;
    ili9341_sand_command(device, 0x3A);    
    ili9341_sand_data(device, data, 8);

    data[0] = 0x00;
    data[1] = 0x18;
    ili9341_sand_command(device, 0xB1);    
    ili9341_sand_data(device, data, 16);
 
    data[0] = 0x08;
    data[1] = 0x82;
    data[2] = 0x27;
    ili9341_sand_command(device, 0xB6);    // Display Function Control 
    ili9341_sand_data(device, data, 24);

    ili9341_sand_command(device, 0x11);    //Exit Sleep 
    vTaskDelay(120 / portTICK_PERIOD_MS);

    ili9341_sand_command(device, 0x29);    //Display on 
}

int16_t RGB888_to_RGB565(uint8_t r,uint8_t g,uint8_t b){
    return b << 11 | g << 5 | r;
}

void ili9341_clear(const ili9341_config_t device, uint16_t px){
    ili9341_sand_command(device, 0x2c);
    uint16_t* frame = (uint16_t *)heap_caps_malloc(32768*sizeof(uint16_t), MALLOC_CAP_DMA);
    for (size_t i = 0; i < 32768; i++){
        frame[i] = px;
    }
    gpio_set_level(device.dc_pin, 1);
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 262144;
    transaction.tx_buffer = frame;
    gpio_set_level(device.dc_pin, 1);
    spi_device_polling_transmit(device.spi_device, &transaction);
    spi_device_polling_transmit(device.spi_device, &transaction);
    spi_device_polling_transmit(device.spi_device, &transaction);
    spi_device_polling_transmit(device.spi_device, &transaction);
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 180224;
    transaction.tx_buffer = frame;
    spi_device_polling_transmit(device.spi_device, &transaction);
    heap_caps_free(frame);
}

void ili9341_show_frame(const ili9341_config_t device, void* frame){
    ili9341_sand_command(device, 0x2c);
    gpio_set_level(device.dc_pin, 1);
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 262144;
    transaction.tx_buffer = frame;
    gpio_set_level(device.dc_pin, 1);
    spi_device_polling_transmit(device.spi_device, &transaction);
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 262144;
    transaction.tx_buffer = frame+32768;
    spi_device_polling_transmit(device.spi_device, &transaction);
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 262144;
    transaction.tx_buffer = frame+65536;
    spi_device_polling_transmit(device.spi_device, &transaction);
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 262144;
    transaction.tx_buffer = frame+98304;
    spi_device_polling_transmit(device.spi_device, &transaction);
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 180224;
    transaction.tx_buffer = frame+131072;
    spi_device_polling_transmit(device.spi_device, &transaction);
    heap_caps_free(frame);
}

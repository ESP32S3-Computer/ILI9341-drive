#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define SPI_MOSI_PIN 15
#define SPI_MISO_PIN 4
#define SPI_CLK_PIN  16
#define SPI_CS_PIN   5
#define DC_PIN 7
#define RST_PIN 6

esp_err_t ILI9341_Sand_Command(const spi_device_handle_t device, const uint8_t command){
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = 8;
    transaction.tx_buffer = &command;
    gpio_set_level(DC_PIN, 0);
    return spi_device_polling_transmit(device, &transaction);
}

// len: data的长度以bit为单位
esp_err_t ILI9341_Sand_Data(const spi_device_handle_t device, const void* data, const size_t length){
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = length;
    transaction.tx_buffer = data;
    gpio_set_level(DC_PIN, 1);
    return spi_device_polling_transmit(device, &transaction);
}

void Address_set(const spi_device_handle_t device, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2){
    // 可优化 int8->int32
    uint8_t data[4];

    data[0] = x1>>8;
    data[1] = x1;
    data[2] = x2>>8;
    data[3] = x2;
    ILI9341_Sand_Command(device, 0x2a);
    ILI9341_Sand_Data(device, data, 32);

    data[0] = y1>>8;
    data[1] = y1;
    data[2] = y2>>8;
    data[3] = y2;
    ILI9341_Sand_Command(device, 0x2b);
    ILI9341_Sand_Data(device, data, 32);
}

void Lcd_Init(const spi_device_handle_t device){
    gpio_set_level(RST_PIN, 1);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    gpio_set_level(RST_PIN, 0);
    vTaskDelay(15 / portTICK_PERIOD_MS);
    gpio_set_level(RST_PIN, 1);
    vTaskDelay(15 / portTICK_PERIOD_MS);

    uint8_t data[8];

    data[0] = 0x39;
    data[1] = 0x2C;
    data[2] = 0x00;
    data[3] = 0x34;
    data[4] = 0x02;
    ILI9341_Sand_Command(device, 0xCB);
    ILI9341_Sand_Data(device, data, 40);

    data[0] = 0x00;
    data[1] = 0XC1;
    data[2] = 0X30;
    ILI9341_Sand_Command(device, 0xCF);
    ILI9341_Sand_Data(device, data, 24);

    data[0] = 0x85;
    data[1] = 0x00;
    data[2] = 0x78;
    ILI9341_Sand_Command(device, 0xE8);
    ILI9341_Sand_Data(device, data, 24);

    data[0] = 0x00;
    data[1] = 0x00;
    ILI9341_Sand_Command(device, 0xEA);
    ILI9341_Sand_Data(device, data, 16);
 
    data[0] = 0x64;
    data[1] = 0x03;
    data[2] = 0X12;
    data[3] = 0X81;
    ILI9341_Sand_Command(device, 0xED);
    ILI9341_Sand_Data(device, data, 32);

    data[0] = 0x20;
    ILI9341_Sand_Command(device, 0xF7);
    ILI9341_Sand_Data(device, data, 8);
  
    data[0] = 0x23;
    ILI9341_Sand_Command(device, 0xC0);    //Power control 
    ILI9341_Sand_Data(device, data, 8);
 
    data[0] = 0x10;
    ILI9341_Sand_Command(device, 0xC1);    //Power control 
    ILI9341_Sand_Data(device, data, 8);

    data[0] = 0x3e;
    data[1] = 0x28;
    ILI9341_Sand_Command(device, 0xC5);    //VCM control 
    ILI9341_Sand_Data(device, data, 16);
 
    data[0] = 0x86;
    ILI9341_Sand_Command(device, 0xC7);    //VCM control2 
    ILI9341_Sand_Data(device, data, 8);
 
    data[0] = 0x20;
    ILI9341_Sand_Command(device, 0x36);    // Memory Access Control 
    ILI9341_Sand_Data(device, data, 8);

    data[0] = 0x55;
    ILI9341_Sand_Command(device, 0x3A);    
    ILI9341_Sand_Data(device, data, 8);

    data[0] = 0x00;
    data[1] = 0x18;
    ILI9341_Sand_Command(device, 0xB1);    
    ILI9341_Sand_Data(device, data, 16);
 
    data[0] = 0x08;
    data[1] = 0x82;
    data[2] = 0x27;
    ILI9341_Sand_Command(device, 0xB6);    // Display Function Control 
    ILI9341_Sand_Data(device, data, 24);

    ILI9341_Sand_Command(device, 0x11);    //Exit Sleep 
    vTaskDelay(120 / portTICK_PERIOD_MS);

    ILI9341_Sand_Command(device, 0x29);    //Display on 
}

int16_t RGB(uint8_t r,uint8_t g,uint8_t b)
{
  return b << 11 | g << 5 | r;
}

void LCD_Clear(const spi_device_handle_t device, uint16_t px){
	ILI9341_Sand_Command(device, 0x2c);
    uint16_t* frame = (uint16_t *)malloc(10000*sizeof(uint16_t));
    for (size_t i = 0; i < 10000; i++){
        frame[i] = px;
    }
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 160000);
    ILI9341_Sand_Data(device, frame, 117776);
    free(frame);
}

void app_main(void){

    gpio_config_t ioConfig = {
		.pin_bit_mask = (1ull << DC_PIN)|(1ull << RST_PIN),
        .mode = GPIO_MODE_OUTPUT,
    	.pull_down_en = 0, // 不下拉
    	.pull_up_en = true,// 不上拉
    	.intr_type = GPIO_INTR_DISABLE, //不启用gpio中断
    };
	gpio_config(&ioConfig);
    

    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 614401,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO); // Enable the DMA feature
    
    spi_device_interface_config_t dev_config = {
        .mode = 0,
        .clock_speed_hz = SPI_MASTER_FREQ_10M, 
        .spics_io_num = SPI_CS_PIN,
        .queue_size = 4096,
	    .duty_cycle_pos = 0,

    };
    spi_device_handle_t device;
    spi_bus_add_device(SPI2_HOST, &dev_config, &device);
    Lcd_Init(device);
    printf("init OK");
    Address_set(device, 0,0,320,240);
    while (true)
    {
        LCD_Clear(device, RGB(31,63,31));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
        LCD_Clear(device, RGB(31,0,0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
        LCD_Clear(device, RGB(0,63,0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
        LCD_Clear(device, RGB(0,0,31));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
        LCD_Clear(device, RGB(0,0,0));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    
    printf("Hello world!\n");
    
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}

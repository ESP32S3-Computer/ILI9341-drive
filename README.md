| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# ILI9341 Driver
ILI9341的ESP32驱动组件，基于ESP-IDF构建系统，理论上所有ESP32都可以使用但是未经过测试

## 食用方法
### ESP-IDF 构建系统
将源代码`git clone`到项目下的组件目录，默认为`components`目录。
然后在代码中`#include "ili9341Driver"`即可使用

### 手动导入
将`src/ili9341Driver.c`和`include/ili9341Driver.h`复制到源代码目录, 然后在代码中`#include "ili9341Driver"`并在编译时添加`src/ili9341Driver.c`文件即可

## API Reference
> ```struct ili9341_config_t```  
>> 用于配置ILI9341的结构体  
>>> 公有成员 (Public Members):  
>>> * `spi_device_handle_t spi_device` -- ILI9341的SPI设备句柄  
>>> * `gpio_num_t dc_pin` -- ILI9341的D/CX引脚(DC)的GPIO编号  
>>> * `gpio_num_t rst_pin` -- ILI9341的RESX引脚(RESET)的GPIO编号  
>>
>>> 使用示例 (Usage Example):  
>>> ```c
>>> gpio_config_t gpio_config = {
>>>     .pin_bit_mask = (1ull << DC_PIN)|(1ull << RST_PIN),
>>>     .mode = GPIO_MODE_OUTPUT,
>>>     .pull_down_en = 0,
>>>     .pull_up_en = true,
>>>     .intr_type = GPIO_INTR_DISABLE,
>>> };
>>> gpio_config(&gpio_config);
>>>     
>>> spi_bus_config_t spi_bus_config = {
>>>     .mosi_io_num = SPI_MOSI_PIN,
>>>     .miso_io_num = SPI_MISO_PIN,
>>>     .sclk_io_num = SPI_CLK_PIN,
>>>     .quadwp_io_num = -1,
>>>     .quadhd_io_num = -1,
>>>     .max_transfer_sz = 65536,
>>> };
>>> spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO); // Enable the DMA feature
>>>     
>>> spi_device_interface_config_t spi_device_config = {
>>>     .mode = 0,
>>>     .clock_speed_hz = SPI_MASTER_FREQ_40M, 
>>>     .spics_io_num = SPI_CS_PIN,
>>>     .queue_size = 4096,
>>>     .duty_cycle_pos = 0,
>>> };
>>> spi_device_handle_t spi;
>>> spi_bus_add_device(SPI2_HOST, &spi_device_config, &spi);
>>> 
>>> ili9341_config_t device = {
>>>     .dc_pin = DC_PIN,
>>>     .rst_pin = RST_PIN,
>>>     .spi_device = spi,
>>> };
>>> ```
---
>`esp_err_t ili9341_sand_command(const ili9341_config_t device, const uint8_t command)`  
>> 用于向ILI9341发送命令  
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
>>> * `command` -- 要发送到ILI9341的命令
>>
>>> 返回 (Returns):
>>> * ESP_OK: 命令发送成功  
>>> 返回其他值则表示发送失败
---
>`esp_err_t ili9341_sand_data(const ili9341_config_t device, const void* data, const size_t length)`  
>> 用于向ILI9341发送数据(命令的参数)  
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
>>> * `data` -- 要发送到ILI9341数据
>>> * `length` -- data参数的长度以byte为单位
>>
>>> 返回 (Returns):
>>> * ESP_OK: 数据发送成功  
>>> 返回其他值则表示发送失败
---
>`esp_err_t ili9341_sand_data(const ili9341_config_t device, const void* data, const size_t length)`  
>> 用于向ILI9341发送数据(命令的参数)  
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
>>> * `data` -- 要发送到ILI9341数据
>>> * `length` -- data参数的长度以byte为单位
>>
>>> 返回 (Returns):
>>> * ESP_OK: 数据发送成功  
>>> 返回其他值则表示发送失败
---
>`void ili9341_init(const ili9341_config_t device)`  
>> 用于向ILI9341发送初始化命令
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
---
>`void ili9341_address_set(const ili9341_config_t device, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)`  
>> 用于向ILI9341设置一个矩形的显存写入空间
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
>>> * `x1` -- 矩形左上角顶点的x坐标
>>> * `y1` -- 矩形左上角顶点的y坐标
>>> * `x2` -- 矩形右下角顶点的x坐标
>>> * `y2` -- 矩形右下角顶点的y坐标
---
>`int16_t RGB888_to_RGB565(uint8_t r,uint8_t g,uint8_t b)`  
>> 该函数目前并不是真正的RGB888_to_RGB565，只是用于设置RGB565的RGB值
>>> 参数 (Parameters):  
>>> * `r` -- 红色(0-32)
>>> * `g` -- 绿色(0-64)
>>> * `b` -- 蓝色(0-32)
>> 
>>> 返回 (Returns): RGB565
---
>`void ili9341_clear(const ili9341_config_t device, uint16_t px)`  
>> 用于将ILI9341的屏幕设置成同一个颜色
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
>>> * `px` -- RGR565
---
>`void ili9341_show_frame(const ili9341_config_t device, void* frame)`  
>> 用于让ILI9341显示一帧图像
>>> 参数 (Parameters):  
>>> * `device` -- ILI9341的配置结构体
>>> * `frame` -- 一个320\*240\*16比特的数组，通常情况下是int16_t(RGB565)的一个320*240的数组

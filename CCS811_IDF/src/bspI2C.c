

#include "bspI2C.h"
#include "sdkconfig.h"



void BSP_I2C_Setup()
{
    i2c_config_t config;

    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = SDA_PIN_NUM;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = SCL_PIN_NUM;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = I2C_MASTER_FREQ;

    esp_err_t err;
    err = i2c_param_config(I2C_MASTER_NUM, &config);
    ESP_ERROR_CHECK(err);
    err = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
    ESP_ERROR_CHECK(err);


}


void BSP_I2C_Write_Byte(uint8_t dev_addres, uint8_t reg_address, uint8_t data)
{
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_addres << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_address, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000/portTICK_RATE_MS);
    ESP_ERROR_CHECK(err);
    i2c_cmd_link_delete(cmd);

}


uint8_t BSP_I2C_Read_Byte(uint8_t dev_address, uint8_t reg_address)
{
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t data;
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_address << 1 | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &reg_address, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000/portTICK_RATE_MS);
    ESP_ERROR_CHECK(err);

    i2c_cmd_link_delete(cmd);

    return data;
}
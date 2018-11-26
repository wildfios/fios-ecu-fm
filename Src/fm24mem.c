#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "fm24mem.h"
#include "string.h"

int fm24_write_block (uint16_t adr, uint8_t *data)
{
  uint32_t buf_len = strlen(data);
  return HAL_I2C_Mem_Write(&hi2c1, FM24_DEV_ADR, adr, I2C_MEMADD_SIZE_16BIT, data, buf_len, 1000 );
}

int fm24_read_block  (uint16_t adr, uint8_t *data, uint16_t len)
{
  return HAL_I2C_Mem_Read(&hi2c1, FM24_DEV_ADR, adr, I2C_MEMADD_SIZE_16BIT, data, len, 1000 );
}

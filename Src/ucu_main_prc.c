#include "stm32f1xx_hal.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fm24mem.h"
#include "string.h"

#include "ucu_main_prc.h"
#include "crankshat_sensor.h"


void HAL_UART_RxCpltCallback (UART_HandleTypeDef *UartHandle) {
  HAL_UART_Transmit(&huart2, "SOME DATA\n", strlen("SOME DATA\n"), 1000);
}



void ecu_start_main_loop () {
  uint8_t data[] = "-------------------";
  uint8_t src[] = "HELLO BMW ECU ";
  uint8_t couner = 0x00;

  fm24_write_block(0x10, &src);
  fm24_read_block(0x10, &data, strlen(data));

  HAL_UART_Transmit(&huart2, &data, strlen(data), 1000);

  strat_crank_capture();

  while(SET)
  {
    HAL_UART_Transmit(&huart2, couner++, 1, 1000);
    HAL_Delay(100);
  }
}


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
#include "pc_interface.h"

/*
 *
 * PB6 -> SCL
 * PB7 -> SDA
 *
 * PA15 -> Krank input
 *
 * */

void ecu_start_main_loop () {
  uint8_t data[] = "-------------------";
  uint8_t src[] = "fios BMW ECU ";

  fm24_write_block(0x10, &src);
  fm24_read_block(0x10, &data, strlen(data));

  strat_crank_capture();
  init_telemetry();

  while(SET) {
  }
}


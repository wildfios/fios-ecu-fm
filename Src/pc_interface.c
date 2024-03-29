#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"

#include "pc_interface.h"
#include "crankshat_sensor.h"

/* TX Data structures */
data_pack dataPack;
telemetry_st telemetryData;
uint8_t signature[] = {0xA5, 0x5A};

/* TX busy flag */
uint8_t busy = SND_FREE;
uint8_t telemtry_busy = SND_FREE;

/* RX Buffer and index pointer */
uint8_t rxBuffer[64];
uint8_t rxPointer = 0;

// test propose only
void dataRecived() {
  telemtry_busy = SND_FREE;
  return;
}

void dataTransmitted() {
  while(busy == SND_BUSY) {}
  //send_data(rxBuffer, 10, &dataRecived);
  read_memory();
  return;
}

void on_byte_transmitted() {
  if (dataPack.size == 0) {
    busy = SND_FREE;
    if (dataPack.callBack != NULL) {
      dataPack.callBack();
    }
    return;
  }
  dataPack.size--;
  USART2->DR = *(dataPack.data + dataPack.size);
}

void on_byte_recived(uint8_t data) {
  rxBuffer[rxPointer] = data;
  if (data == 0xA5) {
    if (rxBuffer[rxPointer - 1] == 0x5A) {
      rxPointer = 0;
      telemtry_busy = SND_BUSY;
      dataTransmitted();
    }
  }
  rxPointer ++;
  if (rxPointer > 63) {
    rxPointer = 0;
  }
  return;
}

void send_data(uint8_t * data, uint16_t size, transferCallBack callBack) {
  if (busy == SND_BUSY) {
    return;
  }
  busy = SND_BUSY;
  dataPack.data = data;
  dataPack.size = size;
  dataPack.callBack = callBack;
  on_byte_transmitted();
}

void send_telemetry() {
//  if (telemtry_busy == SND_BUSY) {
//    return;
//  }
  telemetryData.header = 0xA55A;
  telemetryData.load = 0xFF;
  telemetryData.temerature = 0xFF;
  telemetryData.rpm = get_current_rpm();
  telemetryData.cmdCode = CMD_TELEMETRY;
  telemetryData.signatre = 0xA55A;
  send_data((uint8_t*)&telemetryData, sizeof(telemetry_st), NULL);
}

void read_memory() {
  uint16_t terminator = 0x5AA5;

  uint8_t data[] = "-------------------";
  uint8_t src[] = "HELLO BMW ECU ";

  //fm24_write_block(0x10, &src);
  fm24_read_block(0x10, &data, strlen(data));

  send_data(data, strlen(data), &dataRecived);
//  send_data(terminator, 2, &dataRecived);
}

void init_telemetry() {

  HAL_TIM_Base_Start_IT(&htim7);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}


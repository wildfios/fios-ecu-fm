#include <string.h>

#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "fm24mem.h"
#include "pc_interface.h"
#include "crankshat_sensor.h"


#define UART_HI_TETRADE 1
#define UART_LO_TETRADE 0

#define UART_TX_BSY_FREE 1
#define UART_TX_BSY_BUSY 0

uint8_t  txBuffer[1000] = { 0xA2, 0x52, 0xAF, 0xFF, 0x42, 0x00 };
uint16_t txCounter = 0;
uint16_t txSize = 0;
uint8_t  txBusy    = UART_TX_BSY_FREE;

uint8_t  rxBuffer[1000] = { 0 };
uint16_t rxCounter    = 0;

uint8_t hiTetradeTx = UART_HI_TETRADE;
uint8_t hiTetradeRx = UART_HI_TETRADE;

uint8_t mapBuffer[300];

#pragma pack(push, 1)
typedef struct telemetryData {
	uint8_t  packType;
  uint16_t rpmValue;
	uint16_t engineLoad;
	uint8_t  incorrectSync;
	uint8_t  correctSync;
	uint8_t  endSing;
} telemetryData_t;

#pragma pack(push, 1)
typedef struct mapHeader {
	uint8_t  packType;
  uint16_t xStart;
	uint16_t xStep;
  uint16_t yStart;
	uint16_t yStep;
	uint8_t  demention;
	uint8_t  dementionY;
  uint16_t address;
} mapHeader_t;


void on_data_recived(uint16_t);

void on_byte_transmitted() {
	if (txBusy == UART_TX_BSY_FREE) {
		/* Place here CALLBACK */
		return;
	}

	if (txBuffer[txCounter] != 0x0a) {
		if (hiTetradeTx) {
			hiTetradeTx = UART_LO_TETRADE;
			USART1->DR = txBuffer[txCounter] / 0x10 + 0x10;
		} else {
			hiTetradeTx = UART_HI_TETRADE;
			USART1->DR = txBuffer[txCounter] % 0x10 + 0x10;
			txCounter++;
		}
	} else {
		txBusy = UART_TX_BSY_FREE;
		txCounter = 0;
		USART1->DR = '\n';
	}
}

void on_byte_recived() {
	uint8_t rxByte = USART1->DR;

	if (rxByte == 0x0a) {
		on_data_recived(rxCounter);
		rxCounter = 0;
		return;
	}

	if (hiTetradeRx) {
		rxBuffer[rxCounter] = (rxByte - 0x10 * 10);
		hiTetradeRx = UART_LO_TETRADE;
	} else {
		rxBuffer[rxCounter] += rxByte - 0x10;
		hiTetradeRx = UART_HI_TETRADE;
		rxCounter++;
	}
}

void transmit_buffer() {

}

/*
 * Callbacks for uart operations
 *
 */

void on_data_transmitted() {

}

void send_data(uint16_t size) {
  txCounter = size;
  txBusy = UART_TX_BSY_BUSY;
  on_byte_transmitted();
}

/*
 * Recived data processing
 */
void on_data_recived(uint16_t count) {
  mapHeader_t *header;

  header = (mapHeader_t*)&rxBuffer;

//	if (header->packType == 70) { /* Fetch map request */
//    /* Add here address of block */
//    fm24_read_block(header->address, txBuffer, 100);
//    while(txBusy == UART_TX_BSY_BUSY) {};
//    txBusy = UART_TX_BSY_BUSY;
//    on_byte_transmitted();
//  } else {

//  }
	//  fm24_write_block(0x10, "test wr\n");
}

void send_telemetry() {
  telemetryData_t *data;

  if (txBusy != UART_TX_BSY_BUSY) {
    data = (telemetryData_t*)&txBuffer;
    data->packType = 'S';
    data->rpmValue = get_current_rpm();
    data->engineLoad = 0x0000;
    data->incorrectSync = get_incorect_sync();
    data->correctSync = get_sync_value();
    data->endSing = 0x0a;
    send_data();
	}
}

void init_telemetry() {
	HAL_TIM_Base_Start_IT(&htim7);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

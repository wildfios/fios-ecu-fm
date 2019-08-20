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

uint8_t  arrayTx[1000] = { 0xA2, 0x52, 0xAF, 0xFF, 0x42, 0x00 };
uint16_t txCounter = 0;
uint8_t  txBusy    = UART_TX_BSY_FREE;

uint8_t  rxBuffer[1000] = { 0 };
uint16_t rxCounter    = 0;

uint8_t hiTetradeTx = UART_HI_TETRADE;
uint8_t hiTetradeRx = UART_HI_TETRADE;

uint8_t mapBuffer[300];

typedef struct dataPack {
	uint16_t angle;
	uint16_t count;
	uint16_t crc;
} dataPack_t;

void on_data_recived(uint16_t);


void on_byte_transmitted() {
	if (txBusy == UART_TX_BSY_FREE) {
		/* Place here CALLBACK */
		return;
	}

	if (arrayTx[txCounter] != 0x00) {
		if (hiTetradeTx) {
			hiTetradeTx = UART_LO_TETRADE;
			USART1->DR = arrayTx[txCounter] / 0x10 + 0x10;
		} else {
			hiTetradeTx = UART_HI_TETRADE;
			USART1->DR = arrayTx[txCounter] % 0x10 + 0x10;
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

/*
 * Recived data processing
 */
void on_data_recived(uint16_t count) {
	//if (rxBuffer[0] == 'M') {
	  fm24_write_block(0x10, "test wr\n");//&rxBuffer + 2);
	//} else { //if (rxBuffer[0] == 'M') {
//		fm24_read_block(0x10, &arrayTx, 11);
//		txBusy = UART_TX_BSY_BUSY;
//		on_byte_transmitted();
//	}
//	decoded = indata[3:-1]
//	fuelmap = list(decoded)
//	ser.write('F')




//	if (recvHeader.packType == 'W') {
//		/*
//		 * Get bytes Count and Address
//		 * Write to internal memory
//		 *
//		 * */
//		/* Send resp */
//	} else if (recvHeader.packType == 'R') {
//		/*
//		 * Get count of bytes to read
//		 *
//		 * */
//		/* Send memory dump */
//	}

//	for (int i = 0; i < count; i++) {
//		printf("0x%02x ", rxBuffer[i]);
//	}
//
//	printf("\n\n === rx is done === \n\n");
}

void send_telemetry() {
	if (txBusy != UART_TX_BSY_BUSY) {
//		txBusy = UART_TX_BSY_BUSY;
//		on_byte_transmitted();
		//fm24_read_block(0x10, &arrayTx, 11);
		txBusy = UART_TX_BSY_BUSY;
		on_byte_transmitted();
	}

}

void init_telemetry() {
	HAL_TIM_Base_Start_IT(&htim7);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}


//	telemetryData.header = 0xA55A;
//	telemetryData.load = 0xFF;
//	telemetryData.temerature = 0xFF;
//	telemetryData.rpm = get_current_rpm();
//	telemetryData.cmdCode = CMD_TELEMETRY;
//	telemetryData.signatre = 0xA55A;
//	memcpy(txBuffer, &telemetryData, sizeof(telemetryData));
//	send_data((uint8_t*) &telemetryData, sizeof(telemetry_st), NULL);


///*
// * Testing propose functions
// *
// * */
//int main() {
//	int j = 0;
//
//	txBusy = UART_TX_BSY_BUSY;
//	while (txBusy == UART_TX_BSY_BUSY) {
//		on_byte_transmitted();
//	}
//	printf(" === tx is done === \n\n");
//
//	while (rxEnFlag == 0) {
//		rxByte = arrayRx[j++];
//		on_byte_recived();
//	}
//
//	return 0;
//}
//
///*
//
// for (int i = 0; i < sizeof(arrayTx); i++) {
// printf("0x%02x ", arrayTx[i]);
// printf("0x%01x ", arrayTx[i] / 0x10 + 0x10);
// printf("0x%01x\n", arrayTx[i] % 0x10 + 0x10);
// }
//
// */


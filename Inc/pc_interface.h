/*
 * pc_interface.h
 *
 *  Created on: 27 нояб. 2018
 *      Author: home
 */

#ifndef INC_PC_INTERFACE_H_
#define INC_PC_INTERFACE_H

#define CMD_TELEMETRY 0x01
#define SND_BUSY      1
#define SND_FREE      0

typedef void (*transferCallBack)(void);

#pragma pack(push, 1)
typedef struct {
  uint16_t header;
  uint8_t cmdCode;
  uint32_t rpm;
  uint8_t load;
  uint8_t temerature;
  uint16_t signatre;
} telemetry_st;
#pragma pack(pop)

typedef struct {
  uint8_t* data;
  uint16_t size;
  transferCallBack callBack;
} data_pack;

void init_telemetry();
void send_telemetry();
void on_byte_transmitted();
void on_byte_recived(uint8_t);
void rcv_data_interrupt();
void send_data(uint8_t*, uint16_t, transferCallBack);

#endif /* INC_PC_INTERFACE_H_ */

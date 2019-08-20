/*
 * crankshat_sensor.h
 *
 *  Created on: 24 нояб. 2018
 *      Author: home
 */

#ifndef INC_CRANKSHAT_SENSOR_H_
#define INC_CRANKSHAT_SENSOR_H_

#define FRQ_MUL_FACTOR 2

#define CRANK_SYNC_YES 1
#define CRANK_SYNC_NO 0

#define CRANK_SYNK_PULS_COUNT 58 / 2

#define ODD_PULSE 0
#define EVEN_PULSE 1

#define MAX_TIMER_VALUE 0xFFFF

void     strat_crank_capture();
uint16_t get_current_rpm();
uint8_t  get_incorect_sync();
uint8_t  get_sync_value();


#endif /* INC_CRANKSHAT_SENSOR_H_ */

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

#define CRANK_SYNK_PULS_COUNT 62

void strat_crank_capture();
uint32_t get_current_rpm ();


#endif /* INC_CRANKSHAT_SENSOR_H_ */

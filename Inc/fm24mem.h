/*
 * fm24mem.h
 *
 *  Created on: 26 авг. 2018
 *      Author: home
 */

#ifndef INC_FM24MEM_H_
#define INC_FM24MEM_H_

#define FM24_DEV_ADR 0xA0

int fm24_write_block (uint16_t adr, uint8_t *data);
int fm24_read_block  (uint16_t adr, uint8_t *data, uint16_t len);


#endif /* INC_FM24MEM_H_ */

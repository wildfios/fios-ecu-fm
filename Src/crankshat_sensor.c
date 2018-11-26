#include "crankshat_sensor.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

uint16_t pulsStart = 0;
uint16_t pulsEnd = 0;
uint16_t overCout = 0;

/* Crankshaft sync */
uint8_t isSync = CRANK_SYNC_NO;
int32_t meanPulsWidth = 0;
uint8_t syncCout = 0;

uint16_t angle = 0;
uint16_t crankError = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance==TIM2) {
    overCout ++;
  } else if (htim->Instance==TIM3) {
      angle++;
      if (angle == 10) {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
      } else if (angle == 15) {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
      }
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  uint16_t period = 0;

  if (htim->Instance==TIM2) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
      pulsStart = __HAL_TIM_GetCompare(&htim2, TIM_CHANNEL_1);
      overCout = 0;
    } else {
      pulsEnd = __HAL_TIM_GetCompare(&htim2, TIM_CHANNEL_2);
      period = pulsEnd + (65535 * overCout) - pulsStart;
      if (isSync == CRANK_SYNC_YES) {
	if (period < (meanPulsWidth * 2)) {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
          __HAL_TIM_SET_AUTORELOAD(&htim3, (uint16_t)period / FRQ_MUL_FACTOR);
          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t)period / (FRQ_MUL_FACTOR * 2));
          __HAL_TIM_SET_COUNTER(&htim3, 0);
          crankError = 0;
        } else {
	  if (crankError == 1) {
	    isSync = CRANK_SYNC_NO;
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	  }
	  crankError = 1;
	  angle = 0;
        }
	meanPulsWidth = (meanPulsWidth + period) / 2;
      } else {
	if (meanPulsWidth == 0) {
	  meanPulsWidth = period;
	} else {
	  /* Calc meen value of crank sensor puls width */
	    meanPulsWidth = (meanPulsWidth + period) / 2;
	  if (syncCout == CRANK_SYNK_PULS_COUNT) {
	    syncCout = 0;
	    isSync = CRANK_SYNC_YES;
	    crankError = 0;
	  } else {
	    syncCout++;
	  }
	}
      }
    }

  }
}

void strat_crank_capture() {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}


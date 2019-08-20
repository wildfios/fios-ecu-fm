#include "stm32f1xx_hal.h"
#include "tim.h"

#include "crankshat_sensor.h"

uint16_t pulsStart = 0;
uint16_t pulsEnd = 0;
uint16_t overCout = 0;

/* Crankshaft sync */
uint8_t isSync = CRANK_SYNC_NO;
uint8_t toothIntervalCount = 0;
uint8_t syncCout = 0; 

/* Debug info */
uint8_t incorrectSyncValue = 0;
uint8_t currentSyncValue = 0;

uint16_t angle = 0;
uint16_t crankError = 0;
int32_t  rpmVal = 0;

/*
 * TODO: Add ignition here
 *
 * */
void multimled_frq_tick_hnd() {
  toothIntervalCount ++; /* Estimated pulse counter */
	
  if (angle < 10) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
  } else if (angle > 20) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
  }
  angle++;
}

void set_multipled_frq(uint16_t pulsWidth) {
  __HAL_TIM_SET_AUTORELOAD(&htim3,
      (uint16_t)pulsWidth / FRQ_MUL_FACTOR);       /* Multimplied period for tooth counter */
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,
      (uint16_t)pulsWidth / (FRQ_MUL_FACTOR * 2)); /* Multiplied pulse PWM output. Testing prepose only */
  __HAL_TIM_SET_COUNTER(&htim3, 0);                /* Null the counter */
}

void analyze_crank_period(uint32_t period) {
  syncCout++;

  if (toothIntervalCount > FRQ_MUL_FACTOR * 2) {
    if (syncCout < 30) {
      incorrectSyncValue = syncCout;
      isSync = CRANK_SYNC_NO;
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);   /* CHECK turn on */
    } else {
      currentSyncValue = syncCout;
      isSync = CRANK_SYNC_YES;
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); /* CHECK turn off */
    }
    syncCout = 0;
    angle = 0;
  } else {
    set_multipled_frq(period);
  }
  toothIntervalCount = 0;
}

void process_crank_pulse() {
  uint32_t period = 0;

  if (oddPulsFront == ODD_PULSE) {  //htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    pulsStart = __HAL_TIM_GetCompare(&htim2, TIM_CHANNEL_1);     /* Rising age timer value */
    overCout = 0;
    oddPulsFront = EVEN_PULSE;
  } else {
    pulsEnd = __HAL_TIM_GetCompare(&htim2, TIM_CHANNEL_1);       /* Falling age timer value for mesuring period */
    period = pulsEnd + (MAX_TIMER_VALUE * overCout) - pulsStart; /* Period of tooth interval */
    oddPulsFront = ODD_PULSE;
    analyze_crank_period(period);
  }
}

/* ==== Exported functions ==== */

uint16_t get_current_rpm() {
  return rpmVal != 0 ? 8000000 / rpmVal * 2 : 0;
}

uint8_t get_incorect_sync() {
  return incorrectSyncValue;
}

uint8_t get_sync_value() {
  return currentSyncValue;
}

void strat_crank_capture() {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);   // Test output only
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}

/*  ==== Hardware interrupts ==== */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2)
    process_crank_pulse();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    overCout++;                /* Tooth period timer overcount increment */
  } else if (htim->Instance == TIM3) {
    multimled_frq_tick_hnd();  /* Multiplyed frq tick */
  }
}

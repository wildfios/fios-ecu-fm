#include "stm32f1xx_hal.h"
#include "tim.h"

#include "crankshat_sensor.h"

uint16_t pulsStart = 0;
uint16_t pulsEnd = 0;
uint16_t overCout = 0;

/* Crankshaft sync */
uint8_t isSync = CRANK_SYNC_NO;
int32_t meanPulsWidth = 0;
uint8_t syncCout = 0;

uint16_t angle = 0;
uint16_t crankError = 0;

/* RPM value */
uint32_t rpmVal = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance==TIM2) {
    /*
     * Tooth period timer overcount increment
     * */
    overCout ++;
  } else if (htim->Instance==TIM3) {
      /*
       * Angle mesurment branch
       * */
      angle++;

      /*
       * TODO: Add ignition here
       * */
      if (angle == 10) {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
      } else if (angle > 15) {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
      }
  }
}


/*
 * Called each falling or rising age on crank sensor, calcs period in capture mode
 *
 * */
void process_crank_pulse (TIM_HandleTypeDef *htim) {
  uint16_t period = 0;

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    pulsStart = __HAL_TIM_GetCompare(&htim2, TIM_CHANNEL_1); /* Rising age timer value */
    overCout = 0;                                            /* Timer overcount since rising age */
  } else {
    pulsEnd = __HAL_TIM_GetCompare(&htim2, TIM_CHANNEL_2);   /* Falling age timer value for mesuring period */
    period = pulsEnd + (65535 * overCout) - pulsStart;       /* Period of tooth interval */
    /* ----==== Crank is synced ====---- */
    if (isSync == CRANK_SYNC_YES) {
	/* ---- Normal pulse width, less then two pulse length ---- */
	if (period < (meanPulsWidth * 2)) {
	  meanPulsWidth = (meanPulsWidth + period) / 2;
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);    /* CHECK turn off */
	  rpmVal = meanPulsWidth;                                  /* Storing RPM value for dashboard */
        __HAL_TIM_SET_AUTORELOAD(&htim3, (uint16_t)meanPulsWidth / FRQ_MUL_FACTOR);                    /* Multimplied period for tooth counter */
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t)meanPulsWidth / (FRQ_MUL_FACTOR * 2));  /* Multiplied pulse PWM output. Testing prepose only */
        __HAL_TIM_SET_COUNTER(&htim3, 0);        /* Null the counter */
        crankError = 0; /*  */
      /* ---- Pulse lenght too long REPPER POINT CHECK---- */
      } else {
        /* We have error in crank sync, interval is too long > 4 periods, lost sync */
	  if (crankError == 1) {
	    isSync = CRANK_SYNC_NO;                               /* Lost crank  */
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);   /* CHECK turn on */
	  }
	  /* +++ REPPER point reached +++ new crank sycle, no errors */
	  crankError = 1;  /*  */
	  angle = 0;       /* Null the crank angle */
      }
    /* ----==== Sync branch, lets count (exapmple) 64 tooth to start ignition ====----  */
    } else {
      /*  ---- Init puls width ---- */
      if (meanPulsWidth == 0) {
        meanPulsWidth = period;
        /* ---- Erro in sync, reinit loop ---- */
      } else {
         /* Calc meen value of crank sensor puls width */
         meanPulsWidth = (meanPulsWidth + period) / 2;
         /*
          * ----==== Check if REPPER is correct 64-2 teeth for example ====----
          * Set sync ok flag, ready for ignition
          *
          * */
	if (syncCout == CRANK_SYNK_PULS_COUNT) {
	  syncCout = 0;
	  /* REPPER point done, first crank cycle pass, set ok flag */
	  isSync = CRANK_SYNC_YES;
	  crankError = 0;
	/*
	 * No sync, wait the crank made one cycle
	 * */
	} else syncCout++;
      }
    }
  }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance==TIM2) {
    process_crank_pulse(htim);
  }
}

uint32_t get_current_rpm () {
  return rpmVal;
}

void strat_crank_capture() {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}


/*
 * conf_tc.h
 *
 * Created: 2020-01-21 19:52:36
 *  Author: Måns Lindeberg
 */ 


#ifndef CONF_TC_H_
#define CONF_TC_H_

enum {
	TC0_INDEX = 0,		/* TC module index */
	TC1_INDEX,
	TC2_INDEX,
	TC3_INDEX,
	TC_GEAR_SHIFT_INDEX
};

#define NBR_OF_TC_MOD		(5)	/* Total number of TC modules */

#define TC_CLOCK_SOURCE		(GCLK_GENERATOR_2)
#define TC_CLOCK_PRSCLR		(TC_CLOCK_PRESCALER_DIV256)

#define TC_CLOCK_FREQUENCY	(16000000)	/* TC clock frequency */
#define TC_CLOCK_PRESC_VAL	(256)		/* Prescaler numerical value */
#define TC_CALC_SHIFTS		(2)		/* Shifts for calculating ticks
							per 0.1 ms */
#define TC_MAX_PERIOD		(10000)		/* Maximum period of time in 0.1 ms */
#define TC_MAX_PERIOD_GEAR	(1000)		/* ... for gear shifting */

#endif /* CONF_TC_H_ */
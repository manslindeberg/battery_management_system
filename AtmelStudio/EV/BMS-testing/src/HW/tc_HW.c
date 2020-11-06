/*
 * tc_HW.c
 *
 * Created: 2020-01-21 19:48:36
 *  Author: Måns Lindeberg
 */ 

#include <asf.h>
#include "config\conf_tc.h"
#include "config\conf_typedef.h"

static const uint32_t		tc_ticks_0p1_ms = ((TC_CLOCK_FREQUENCY
	/ TC_CLOCK_PRESC_VAL) << TC_CALC_SHIFTS) / 10000;	/* Ticks for 0.1 ms
									(shifted) */
/*
 * Array declarations.
 *
 * Description: Arrays for sharing the same functions between
 *	all TC modules.
 */
static Tc*			tc_hw_mod[NBR_OF_TC_MOD] = {	/* Hardware modules */
	TC0,
	TC1,
	TC2,
	TC3,
	TC4
};

static struct tc_module		tc_instance[NBR_OF_TC_MOD];	/* Module structs */
static void_func_ptr		tc_callb_func[NBR_OF_TC_MOD];	/* Callback functions */

/*
 * tc_callb_internal - Internal callback function used to
 *	restart or stop a timer.
 *
 * @param[in] module: Module struct containing TC timer data.
 */
static void tc_callb_internal(struct tc_module* module)
{
	static uint_fast8_t	tc_index;	/* Index of TC module */

	for (tc_index = 0; tc_index < (NBR_OF_TC_MOD - 1); tc_index++) {
		if (module->hw == tc_hw_mod[tc_index]) {
			break;
		}
	}
	if (tc_index != TC_GEAR_SHIFT_INDEX) {
		tc_start_counter(&tc_instance[tc_index]);	/* Restart timer */
	} else {
		tc_stop_counter(&tc_instance[tc_index]);	/* Stop timer */
	}
	tc_callb_func[tc_index]();	/* Call external callb func */
}

/*
 * _tc_init - Internal init help function.
 *
 * @param[in] tc_index: Index of TC module to initialize.
 * @param[in] callb_func: Callback function that returns void
 *	and takes no argument.
 * @param[in] compare: Timer compare value in 0.1 ms resolution.
 *	Max value is 10000, which corresponds to one second.
 */
static void _tc_init(const uint8_t tc_index, const void_func_ptr callb_func,
	const uint16_t compare)
{
	if (compare > TC_MAX_PERIOD) {
		return;
	}
	static struct tc_config tc_conf;	/* TC config struct */
	tc_get_config_defaults(&tc_conf);

	tc_conf.counter_size		= TC_COUNTER_SIZE_16BIT;
	tc_conf.clock_source		= TC_CLOCK_SOURCE;
	tc_conf.clock_prescaler		= TC_CLOCK_PRSCLR;
	tc_conf.counter_16_bit.compare_capture_channel[0]
		= (tc_ticks_0p1_ms * compare) >> TC_CALC_SHIFTS;
	tc_init(&tc_instance[tc_index], tc_hw_mod[tc_index], &tc_conf);
	tc_enable(&tc_instance[tc_index]);
	/*tc_stop_counter(&tc_instance[tc_index]);*/

	tc_register_callback(&tc_instance[tc_index],
		tc_callb_internal, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance[tc_index], TC_CALLBACK_CC_CHANNEL0);
	tc_callb_func[tc_index] = callb_func;	/* Store callback function */
}

void tc0_init(const void_func_ptr callb_func, const uint16_t compare)
{
	_tc_init(TC0_INDEX, callb_func, compare);
}

void tc1_init(const void_func_ptr callb_func, const uint16_t compare)
{
	_tc_init(TC1_INDEX, callb_func, compare);
}

void tc2_init(const void_func_ptr callb_func, const uint16_t compare)
{
	_tc_init(TC2_INDEX, callb_func, compare);
}

void tc3_init(const void_func_ptr callb_func, const uint16_t compare)
{
	_tc_init(TC3_INDEX, callb_func, compare);
}

void tc_gear_shift_init(const void_func_ptr callb_func)
{
	_tc_init(TC_GEAR_SHIFT_INDEX, callb_func, TC_MAX_PERIOD_GEAR);
}

void tc0_start(void)
{
	tc_start_counter(&tc_instance[TC0_INDEX]);
}

void tc1_start(void)
{
	tc_start_counter(&tc_instance[TC1_INDEX]);
}

void tc2_start(void)
{
	tc_start_counter(&tc_instance[TC2_INDEX]);
}

void tc3_start(void)
{
	tc_start_counter(&tc_instance[TC3_INDEX]);
}

bool tc_gear_shift_start(const uint16_t compare)
{
	if ((compare == 0) || (compare > TC_MAX_PERIOD_GEAR)) {
		return false;		/* Check argument */
	}
	tc_set_compare_value(&tc_instance[TC_GEAR_SHIFT_INDEX], 0,
		(tc_ticks_0p1_ms * compare) >> TC_CALC_SHIFTS);
	tc_start_counter(&tc_instance[TC_GEAR_SHIFT_INDEX]);
	return true;
}

void tc0_stop(void)
{
	tc_stop_counter(&tc_instance[TC0_INDEX]);
}

void tc1_stop(void)
{
	tc_stop_counter(&tc_instance[TC1_INDEX]);
}

void tc2_stop(void)
{
	tc_stop_counter(&tc_instance[TC2_INDEX]);
}

void tc3_stop(void)
{
	tc_stop_counter(&tc_instance[TC3_INDEX]);
}

long count_value(const uint8_t tc_index)
{
	return tc_get_count_value(&tc_instance[tc_index])/2.048;
}
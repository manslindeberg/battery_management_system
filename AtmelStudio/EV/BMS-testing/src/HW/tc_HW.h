/*
 * tc_HW.h
 *
 * Created: 2020-01-21 20:19:46
 *  Author: Måns Lindeberg
 */ 

#include "config/conf_typedef.h"
#include "config/conf_tc.h"
#ifndef TC_HW_H_
#define TC_HW_H_

/*
 * tc0_init - Initialize Timer/Counter TC0.
 *
 * @param[in] callb_func: Callback function that returns void
 *	and takes no argument.
 * @param[in] compare: Timer compare value in 0.1 ms resolution.
 *	Max value is 10000, which corresponds to one second.
 */
void tc0_init(const void_func_ptr callb_func, const uint16_t compare);

/*
 * tc1_init - Initialize Timer/Counter TC1.
 *
 * @param[in] callb_func: Callback function that returns void
 *	and takes no argument.
 * @param[in] compare: Timer compare value in 0.1 ms resolution.
 *	Max value is 10000, which corresponds to one second.
 */
void tc1_init(const void_func_ptr callb_func, const uint16_t compare);

/*
 * tc2_init - Initialize Timer/Counter TC2.
 *
 * @param[in] callb_func: Callback function that returns void
 *	and takes no argument.
 * @param[in] compare: Timer compare value in 0.1 ms resolution.
 *	Max value is 10000, which corresponds to one second.
 */
void tc2_init(const void_func_ptr callb_func, const uint16_t compare);

/*
 * tc3_init - Initialize Timer/Counter TC3.
 *
 * @param[in] callb_func: Callback function that returns void
 *	and takes no argument.
 * @param[in] compare: Timer compare value in 0.1 ms resolution.
 *	Max value is 10000, which corresponds to one second.
 */
void tc3_init(const void_func_ptr callb_func, const uint16_t compare);

/*
 * tc_gear_shift_init - Initialize Timer/Counter TC4 used for
 *	gear shifting.
 *
 * @param[in] callb_func: Callback function that returns void
 *	and takes no argument.
 */
void tc_gear_shift_init(const void_func_ptr callb_func);

/*
 * tc0_start - Start Timer/Counter TC0.
 */
void tc0_start(void);

/*
 * tc1_start - Start Timer/Counter TC1.
 */
void tc1_start(void);

/*
 * tc2_start - Start Timer/Counter TC2.
 */
void tc2_start(void);

/*
 * tc3_start - Start Timer/Counter TC3.
 */
void tc3_start(void);

/*
 * tc_gear_shift_start - Start Timer/Counter TC4 used for
 *	gear shifting.
 *
 * @param[in] compare: Timer compare value in 0.1 ms resolution.
 *	Max value is 1000, which corresponds to 100 ms.
 * @return Whether the argument is valid or not as a boolean value.
 */
bool tc_gear_shift_start(const uint16_t compare);

/*
 * tc0_stop - Stop Timer/Counter TC0.
 */
void tc0_stop(void);

/*
 * tc1_stop - Stop Timer/Counter TC1.
 */
void tc1_stop(void);

/*
 * tc2_stop - Stop Timer/Counter TC2.
 */
void tc2_stop(void);

/*
 * tc3_stop - Stop Timer/Counter TC3.
 */
void tc3_stop(void);

long count_value(const uint8_t tc_index);

#endif /* TC_HW_H_ */
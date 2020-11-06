/*
 * conf_typedef.h - Typedef configuration.
 *
 * This file is part of the LFS-17 library. This library is used to manage
 * the Control Units belonging to the Lund Formula Student 2016/17 vehicle.
 *
 * This library is strictly written for the Atmel SAM C21G microcontroller
 * based on ARM Cortex-M0+. This library is itself based on the ASF library
 * provided by Atmel.
 *
 * @see <http://www.atmel.com/devices/ATSAMC21G18A.aspx>
 * @see <http://asf.atmel.com/docs/latest/>
 *
 * Author: David Bengtsson <k.david.bengtsson@gmail.com>
 */

#ifndef LFS_CONF_TYPEDEF_H
#define LFS_CONF_TYPEDEF_H

/*
 * Structs.
 */
struct can_frame {		/* CAN message struct */
	uint32_t	id;		/* Message ID */
	uint8_t*	data;		/* Pointer to message data */
	uint8_t		dlc;		/* Message data length (bytes) */
};

/*
 * Function pointers.
 */
typedef void (*adc_func_ptr)(const uint16_t);		/* ADC */
typedef void (*can_func_ptr)(struct can_frame *const);	/* CAN */
typedef void (*void_func_ptr)(void);		/* Void function pointer */

#endif /* LFS_CONF_TYPEDEF_H */

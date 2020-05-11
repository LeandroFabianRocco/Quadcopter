/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_i2c.h"
#include "fsl_pit.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* BOARD_InitPeripherals defines for I2C0 */
/* Definition of peripheral ID */
#define I2C0_PERIPHERAL I2C0
/* Definition of the clock source */
#define I2C0_CLOCK_SOURCE I2C0_CLK_SRC
/* Definition of the clock source frequency */
#define I2C0_CLK_FREQ CLOCK_GetFreq(I2C0_CLOCK_SOURCE)
/* BOARD_InitPeripherals defines for I2C1 */
/* Definition of peripheral ID */
#define I2C1_PERIPHERAL I2C1
/* Definition of the clock source */
#define I2C1_CLOCK_SOURCE I2C1_CLK_SRC
/* Definition of the clock source frequency */
#define I2C1_CLK_FREQ CLOCK_GetFreq(I2C1_CLOCK_SOURCE)
/* BOARD_InitPeripherals defines for PIT */
/* Definition of peripheral ID. */
#define PIT_PERIPHERAL PIT
/* Definition of clock source. */
#define PIT_CLOCK_SOURCE kCLOCK_BusClk
/* Definition of clock source frequency. */
#define PIT_CLK_FREQ 4000000UL
/* Definition of ticks count for channel 0 - deprecated. */
#define PIT_0_TICKS 499U
/* Definition of channel number for channel 0. */
#define PIT_0 kPIT_Chnl_0
/* PIT interrupt vector ID (number). */
#define PIT_0_IRQN PIT0_IRQn
/* PIT interrupt handler identifier. */
#define PIT_0_IRQHANDLER PIT0_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const i2c_master_config_t I2C0_config;
extern const i2c_master_config_t I2C1_config;
extern const pit_config_t PIT_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */

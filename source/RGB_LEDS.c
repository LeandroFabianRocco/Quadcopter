/************************************************************
 * RGB_LEDS.c
 *
 *  Created on: 14 abr. 2019
 *      Author: leandro
 ***********************************************************/

#include "RGB_LEDS.h"

/*********************************************************************************************
 * @brief RGB LED initialization
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void RGB_LED_init(void)
{
	// Define the init structure for the output LEDs pins
	gpio_pin_config_t led_config = {
		kGPIO_DigitalOutput, 1,
	};
	// Inicialización de los LEDs en alto
	GPIO_PinInit(RED_LED_GPIO, RED_LED, &led_config);
	GPIO_PinInit(GREEN_LED_GPIO, GREEN_LED, &led_config);
	GPIO_PinInit(BLUE_LED_GPIO, BLUE_LED, &led_config);
	GPIO_PortSet(BLUE_LED_GPIO, 1u << BLUE_LED);
	GPIO_PortSet(RED_LED_GPIO, 1u << RED_LED);
	GPIO_PortSet(GREEN_LED_GPIO, 1u << GREEN_LED);
}


/*********************************************************************************************
 * @brief Turning on RED LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void RedLEDon(void)
{
	GPIO_PortClear(RED_LED_GPIO, 1u << RED_LED);
}

/*********************************************************************************************
 * @brief Turning off RED LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void RedLEDoff(void)
{
	GPIO_PortSet(RED_LED_GPIO, 1u << RED_LED);
}


/*********************************************************************************************
 * @brief Turning on GREEN LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void GreenLEDon(void)
{
	GPIO_PortClear(GREEN_LED_GPIO, 1u << GREEN_LED);
}

/*********************************************************************************************
 * @brief Turning off GREEN LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void GreenLEDoff(void)
{
	GPIO_PortSet(GREEN_LED_GPIO, 1u << GREEN_LED);
}


/*********************************************************************************************
 * @brief Turning on BLUE LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void BlueLEDon(void)
{
	GPIO_PortClear(BLUE_LED_GPIO, 1u << BLUE_LED);
}


/*********************************************************************************************
 * @brief Turning off BLUE LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void BlueLEDoff(void)
{
	GPIO_PortSet(BLUE_LED_GPIO, 1u << BLUE_LED);
}



/*********************************************************************************************
 * @brief Toggle RED LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void toggleRedLED(void)
{
	GPIO_PortToggle(RED_LED_GPIO, 1u << RED_LED);
}

/*********************************************************************************************
 * @brief Toggle GREEN LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void toggleGreenLED(void)
{
	GPIO_PortToggle(GREEN_LED_GPIO, 1u << GREEN_LED);
}


/*********************************************************************************************
 * @brief Toggle BLUE LED
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void toggleBlueLED(void)
{
	GPIO_PortToggle(BLUE_LED_GPIO, 1u << BLUE_LED);
}







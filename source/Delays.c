/*********************************************************************************************
 * Delays.c
 *
 *	Delays in milliseconds
 *
 *  Created on: 17 mar. 2019
 *      Author: Leandro
 *********************************************************************************************/

#include "Delays.h"


volatile uint32_t g_systickCounter;


/*********************************************************************************************
 * @brief SysTick handler
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}


/*********************************************************************************************
 * @brief Delay in milliseconds
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while(g_systickCounter != 0U){}
}


/*********************************************************************************************
 * @brief SysTick initialization
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void SysTick_init(void)
{
	/* Set systick reload value to generate 1ms interrupt */
	if(SysTick_Config(SystemCoreClock / 1000U))
	{
		while(1)
		{
		}
	}
}


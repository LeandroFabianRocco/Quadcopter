/*
 * UART$_DMA.h
 *
 *  Created on: 29 abr. 2020
 *      Author: leandro
 */

#ifndef UART4_DMA_H_
#define UART4_DMA_H_

#include "fsl_uart_edma.h"
#include "fsl_dmamux.h"



/*******************************************************************************
 * Definitions
 ******************************************************************************/

// UART definitions
#define UART UART4
#define UART_CLKSRC UART4_CLK_SRC
#define UART_CLK_FREQ CLOCK_GetFreq(UART4_CLK_SRC)
// UART DMA definitions
#define UART_RX_DMA_CHANNEL 0U
#define UART_DMAMUX_BASEADDR DMAMUX0
#define UART_DMA_BASEADDR DMA0
#define UART_RX_DMA_REQUEST  kDmaRequestMux0UART4
// Ring buffer size definition
#define UART_RING_BUFFER_SIZE 11



/*********************************************************************************************
 * @brief Initialiize the UART module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void InitUART4(void);


/*********************************************************************************************
 * @brief Initialize the EDMA configuration
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void InitEDMA(void);


/*********************************************************************************************
 * @brief Start ring buffer configuration
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void StartRingBufferEDMA(void);


/*********************************************************************************************
 * @brief Get how many bytes in ring buffer
 *
 * @param void
 *
 * @return Number of bytes in ring buffer
 *********************************************************************************************/
uint32_t GetRingBufferLengthEDMA(void);


/*********************************************************************************************
 * @brief Read the characters from ring buffer
 *
 * @param DMA ring buffer
 * @param local buffer
 * @param number of bytes to read
 *
 * @return void
 *********************************************************************************************/
void ReadRingBufferEDMA(uint8_t *ringBuffer, uint8_t *receiveBuffer, uint32_t length);


/*********************************************************************************************
 * @brief UART user callback
 *
 * @param UART module
 * @param UART handle
 * @param UART status
 * @param user data
 *
 * @return void
 *********************************************************************************************/
void UART_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData);


/*********************************************************************************************
 * @brief Move data from DMA ring buffer to local buffer
 *
 * @param DMA buffer
 * @param local buffer
 *
 * @return void
 *********************************************************************************************/
void MoveDataToLocalBuffer(uint8_t *rxBuffer, uint32_t byteCount);


#endif /* UART4_DMA_H_ */

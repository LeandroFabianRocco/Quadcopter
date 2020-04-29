/*
 * UAR4_DMA.c
 *
 *  Created on: 29 abr. 2020
 *      Author: leandro
 */

#include "UART4_DMA.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
uart_edma_handle_t g_uartEdmaHandle;
edma_handle_t g_uartRxEdmaHandle;

volatile bool txOnGoing                      = false;
volatile bool rxIdleLineDetected             = false;
volatile bool timeoutFlag                    = false;
volatile uint32_t ringBufferIndex            = 0U;

AT_NONCACHEABLE_SECTION_ALIGN(uint8_t g_ringBuffer[UART_RING_BUFFER_SIZE], 16);
/* Allocate TCD memory poll with ring buffer used. */
AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcdMemoryPoolPtr[1], sizeof(edma_tcd_t));



/*********************************************************************************************
 * @brief Initialiize the UART module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void InitUART4(void)
{
    uart_config_t uartConfig;

    UART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps = 9600;
    uartConfig.enableTx     = false;
    uartConfig.enableRx     = true;

    UART_Init(UART, &uartConfig, UART_CLK_FREQ);
}


/*********************************************************************************************
 * @brief Initialize the EDMA configuration
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void InitEDMA(void)
{
    edma_config_t config;

    /* Init DMAMUX */
    DMAMUX_Init(UART_DMAMUX_BASEADDR);
    /* Set channel for UART */
    DMAMUX_SetSource(UART_DMAMUX_BASEADDR, UART_RX_DMA_CHANNEL, UART_RX_DMA_REQUEST);
    DMAMUX_EnableChannel(UART_DMAMUX_BASEADDR, UART_RX_DMA_CHANNEL);

    /* Init the EDMA module */
    EDMA_GetDefaultConfig(&config);
    EDMA_Init(UART_DMA_BASEADDR, &config);
    //EDMA_CreateHandle(&g_uartTxEdmaHandle, EXAMPLE_UART_DMA_BASEADDR, UART_TX_DMA_CHANNEL);
    EDMA_CreateHandle(&g_uartRxEdmaHandle, UART_DMA_BASEADDR, UART_RX_DMA_CHANNEL);

    /* Create UART DMA handle. */
    //UART_TransferCreateHandleEDMA(UART, &g_uartEdmaHandle, UART_UserCallback, NULL, &g_uartRxEdmaHandle, NULL);
}



/*********************************************************************************************
 * @brief Start ring buffer configuration
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void StartRingBufferEDMA(void)
{
    edma_transfer_config_t xferConfig;

    /* Install TCD memory. */
    EDMA_InstallTCDMemory(&g_uartRxEdmaHandle, tcdMemoryPoolPtr, 1);

    /* Prepare transfer to receive data to ring buffer. */
    EDMA_PrepareTransfer(&xferConfig, (void *)UART_GetDataRegisterAddress(UART), sizeof(uint8_t), g_ringBuffer,
                         sizeof(uint8_t), sizeof(uint8_t), UART_RING_BUFFER_SIZE, kEDMA_PeripheralToMemory);

    /* Submit transfer. */
    g_uartRxEdmaHandle.tcdUsed = 1U;
    g_uartRxEdmaHandle.tail    = 0U;
    EDMA_TcdReset(&g_uartRxEdmaHandle.tcdPool[0U]);
    EDMA_TcdSetTransferConfig(&g_uartRxEdmaHandle.tcdPool[0U], &xferConfig, tcdMemoryPoolPtr);

    /* Enable major interrupt for calculating the received bytes. */
    g_uartRxEdmaHandle.tcdPool[0U].CSR |= DMA_CSR_INTMAJOR_MASK;

    /* There is no live chain, TCD block need to be installed in TCD registers. */
    EDMA_InstallTCD(UART_DMA_BASEADDR, UART_RX_DMA_CHANNEL, &g_uartRxEdmaHandle.tcdPool[0U]);

    /* Start EDMA transfer. */
    EDMA_StartTransfer(&g_uartRxEdmaHandle);

    /* Enable UART RX EDMA. */
    UART_EnableRxDMA(UART, true);
}


/*********************************************************************************************
 * @brief Get how many bytes in ring buffer
 *
 * @param void
 *
 * @return Number of bytes in ring buffer
 *********************************************************************************************/
uint32_t GetRingBufferLengthEDMA(void)
{
    uint32_t regPrimask    = 0U;
    uint32_t receivedBytes = 0U;

    /* Disable IRQ, protect ring buffer. */
    regPrimask = DisableGlobalIRQ();

    receivedBytes = UART_RING_BUFFER_SIZE
    		- EDMA_GetRemainingMajorLoopCount(UART_DMA_BASEADDR, UART_RX_DMA_CHANNEL);

    /* If the received bytes is less than index value, it means the ring buffer has reached
     *  it boundary. */
    if (receivedBytes < ringBufferIndex)
    {
        receivedBytes += UART_RING_BUFFER_SIZE;
    }

    receivedBytes -= ringBufferIndex;

    /* Enable IRQ if previously enabled. */
    EnableGlobalIRQ(regPrimask);

    return receivedBytes;
}

/*********************************************************************************************
 * @brief Read the characters from ring buffer
 *
 * @param DMA ring buffer
 * @param local buffer
 * @param number of bytes to read
 *
 * @return void
 *********************************************************************************************/
void ReadRingBufferEDMA(uint8_t *ringBuffer, uint8_t *receiveBuffer, uint32_t length)
{
    assert(ringBuffer);
    assert(receiveBuffer);
    assert(length);

    uint32_t index = length;

    /* If length if larger than ring buffer size, it means overflow occurred,
     * need to reset the ringBufferIndex. */
    if (length > UART_RING_BUFFER_SIZE)
    {
        ringBufferIndex = ((ringBufferIndex + length) % UART_RING_BUFFER_SIZE);
        index           = UART_RING_BUFFER_SIZE;
    }

    while (index)
    {
        *(receiveBuffer++) = ringBuffer[ringBufferIndex++];
        if (ringBufferIndex == UART_RING_BUFFER_SIZE)
        {
            ringBufferIndex = 0U;
        }
        index--;
    }
}

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
void UART_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
        txOnGoing = false;
    }
}


/*********************************************************************************************
 * @brief Move data from DMA ring buffer to local buffer
 *
 * @param DMA buffer
 * @param local buffer
 *
 * @return void
 *********************************************************************************************/
void MoveDataToLocalBuffer(uint8_t *rxBuffer, uint32_t byteCount)
{
	ReadRingBufferEDMA(g_ringBuffer, rxBuffer, byteCount);
}





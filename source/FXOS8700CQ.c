/*
 * FXOS8700CQ.c
 *
 *  Created on: 25 ene. 2020
 *      Author: leandro
 */

#include "FXOS8700CQ.h"


/*********************************************************************************************
 * Variable definitions
 *********************************************************************************************/
volatile bool completionFlag = false;
volatile bool nakFlag        = false;
i2c_master_handle_t g_m_handle;


/*********************************************************************************************
 * @brief I2C master callback
 *
 * @param base I2C peripheral base address.
 * @param I2C master handle
 * @param status success flag
 * @param user data
 *
 * @return void
 *********************************************************************************************/
void FXOS8700CQ_i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
        completionFlag = true;
    }
    /* Signal transfer success when received success status. */
    if ((status == kStatus_I2C_Nak) || (status == kStatus_I2C_Addr_Nak))
    {
        nakFlag = true;
    }
}


/*********************************************************************************************
 * @brief Gets the WHO_AM_I value
 *
 * @param void
 *
 * @return status flag. Return true if no error
 *********************************************************************************************/
bool FXOS8700CQ_ReadSensorWhoAmI(void)
{
	uint8_t who_am_i_reg          = FXOS8700CQ_WHO_AM_I;
	uint8_t who_am_i_value        = 0x00;
	bool find_device              = false;
	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	// START + Slave_address (write_bit); Reg_address
	masterXfer.slaveAddress   = FXOS8700CQ_DEVICE_ADDRESS;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = &who_am_i_reg;
	masterXfer.dataSize       = 1;
	masterXfer.flags          = kI2C_TransferNoStopFlag;
	I2C_MasterTransferNonBlocking(I2C0, &g_m_handle, &masterXfer);

	while ((!nakFlag) && (!completionFlag)){}
	nakFlag = false;

	if (completionFlag == true)
	{
		completionFlag     = false;
		find_device        = true;
	}

	if (find_device == true)
	{
		// START + Slave_address (read_bit);
		masterXfer.direction      = kI2C_Read;
		masterXfer.subaddress     = 0;
		masterXfer.subaddressSize = 0;
		masterXfer.data           = &who_am_i_value;
		masterXfer.dataSize       = 1;
		masterXfer.flags          = kI2C_TransferRepeatedStartFlag;
		I2C_MasterTransferNonBlocking(I2C0, &g_m_handle, &masterXfer);

		while ((!nakFlag) && (!completionFlag)){}
		nakFlag = false;
	}

	completionFlag = false;
	if (who_am_i_value == FXOS8700_WHOAMI_VALUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*********************************************************************************************
 * @brief Write sensor register
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param register address
 * @param data value address
 *
 * @return status flag. True if success
 *********************************************************************************************/
bool FXOS8700CQ_WriteAccelReg(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress   = device_addr;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = &value;
    masterXfer.dataSize       = 1;
    masterXfer.flags          = kI2C_TransferDefaultFlag;


    I2C_MasterTransferNonBlocking(I2C0, &g_m_handle, &masterXfer);

    /*  wait for transfer completed. */
    while ((!nakFlag) && (!completionFlag))
    {
    }

    nakFlag = false;

    if (completionFlag == true)
    {
        completionFlag = false;
        return true;
    }
    else
    {
        return false;
    }
}


/*********************************************************************************************
 * @brief Read sensor register
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param register address
 * @param pointer to data read
 * @param data size
 *
 * @return status flag. True if success
 *********************************************************************************************/
bool FXOS8700CQ_ReadAccelRegs(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress   = device_addr;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    I2C_MasterTransferNonBlocking(I2C0, &g_m_handle, &masterXfer);

    /*  wait for transfer completed. */
    while ((!nakFlag) && (!completionFlag))
    {
    }

    nakFlag = false;

    if (completionFlag == true)
    {
        completionFlag = false;
        return true;
    }
    else
    {
        return false;
    }
}


/*********************************************************************************************
 * @brief Initialize FXOS8700CQ module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void FXOS8700CQ_Init(void)
{
	I2C_MasterTransferCreateHandle(I2C0, &g_m_handle, FXOS8700CQ_i2c_master_callback, NULL);
}


/*********************************************************************************************
 * @brief Configure FXOS8700CQ module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void FXOS8700CQ_Configure_Device(void)
{
	uint8_t databyte  = 0;
	uint8_t write_reg = 0;

	// Acelerómetro
	write_reg = FXOS8700CQ_CTRL_REG1;
	databyte  = 0;
	FXOS8700CQ_WriteAccelReg(I2C0, FXOS8700CQ_DEVICE_ADDRESS, write_reg, databyte);
	// +- 4g
	write_reg = FXOS8700CQ_XYZ_DATA_CFG;
	databyte  = 0x01;
	FXOS8700CQ_WriteAccelReg(I2C0, FXOS8700CQ_DEVICE_ADDRESS, write_reg, databyte);
	// Accel disabled
	write_reg = FXOS8700CQ_CTRL_REG1;
	databyte  = 0x0D;
	FXOS8700CQ_WriteAccelReg(I2C0, FXOS8700CQ_DEVICE_ADDRESS, write_reg, databyte);

	// Magnetómetro
	write_reg = FXOS8700CQ_M_CTRL_REG1;
	databyte = 3;
	FXOS8700CQ_WriteAccelReg(I2C0, FXOS8700CQ_DEVICE_ADDRESS, write_reg, databyte);
}

/*********************************************************************************************
 * @brief Read accelerometer data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return STATUS byte
 *********************************************************************************************/
uint8_t FXOS8700CQ_Read_Accel_Data(I2C_Type *base, uint8_t device_addr, int16_t *xyz_accel)
{
	uint8_t readBuff[7];
	FXOS8700CQ_ReadAccelRegs(I2C0, FXOS8700CQ_DEVICE_ADDRESS, FXOS8700CQ_STATUS, readBuff, 7);
	xyz_accel[0] = ((int16_t)(((readBuff[1] * 256U) | readBuff[2]))) / 4U;
	xyz_accel[1] = ((int16_t)(((readBuff[3] * 256U) | readBuff[4]))) / 4U;
	xyz_accel[2] = ((int16_t)(((readBuff[5] * 256U) | readBuff[6]))) / 4U;
	return readBuff[0];
}


/*********************************************************************************************
 * @brief Read magnetometer data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return STATUS byte
 *********************************************************************************************/
uint8_t FXOS8700CQ_Read_Magnet_Data(I2C_Type *base, uint8_t device_addr, int16_t *xyz_magnet)
{
	uint8_t readBuff[7];
	FXOS8700CQ_ReadAccelRegs(I2C0, FXOS8700CQ_DEVICE_ADDRESS, FXOS8700CQ_STATUS, readBuff, 7);
	xyz_magnet[0] = ((int16_t)(((readBuff[1] * 256U) | readBuff[2]))) / 4U;
	xyz_magnet[1] = ((int16_t)(((readBuff[3] * 256U) | readBuff[4]))) / 4U;
	xyz_magnet[2] = ((int16_t)(((readBuff[5] * 256U) | readBuff[6]))) / 4U;
	return readBuff[0];
}













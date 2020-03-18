/*
 * MPU6050.c
 *
 *  Created on: 1 feb. 2020
 *      Author: leandro
 */



#include "MPU6050.h"


/*********************************************************************************************
 * Variable definitions
 *********************************************************************************************/
volatile bool MPU6050_nakFlag        = false;
volatile bool MPU6050_completionFlag = false;
i2c_master_handle_t mpu_g_m_handle;


/*********************************************************************************************
 * @brief Initialize MPU6050 module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Init(void)
{
	I2C_MasterTransferCreateHandle(I2C1, &mpu_g_m_handle, MPU6050_i2c_master_callback, NULL);
}

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
void MPU6050_i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
        MPU6050_completionFlag = true;
    }
    /* Signal transfer success when received success status. */
    if ((status == kStatus_I2C_Nak) || (status == kStatus_I2C_Addr_Nak))
    {
        MPU6050_nakFlag = true;
    }
}


/*********************************************************************************************
 * @brief Gets the WHO_AM_I value
 *
 * @param void
 *
 * @return status flag. Return true if no error
 *********************************************************************************************/
bool MPU6050_ReadSensorWhoAmI(void)
{
	uint8_t who_am_i_reg          = MPU6050_WHO_AM_I;
	uint8_t who_am_i_value        = 0x00;
	bool find_device              = false;
	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	// START + Slave_address (write_bit); Reg_address
	masterXfer.slaveAddress   = MPU6050_DEVICE_ADDRESS_0;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = &who_am_i_reg;
	masterXfer.dataSize       = 1;
	masterXfer.flags          = kI2C_TransferNoStopFlag;
	I2C_MasterTransferNonBlocking(I2C1, &mpu_g_m_handle, &masterXfer);

	while ((!MPU6050_nakFlag) && (!MPU6050_completionFlag)){}
	MPU6050_nakFlag = false;

	if (MPU6050_completionFlag == true)
	{
		MPU6050_completionFlag     = false;
		find_device        = true;
	}

	if (find_device == true)
	{
		// START + Slave_address (read_bit); recibo dato
		masterXfer.direction      = kI2C_Read;
		masterXfer.subaddress     = 0;
		masterXfer.subaddressSize = 0;
		masterXfer.data           = &who_am_i_value;
		masterXfer.dataSize       = 1;
		masterXfer.flags          = kI2C_TransferRepeatedStartFlag;
		I2C_MasterTransferNonBlocking(I2C1, &mpu_g_m_handle, &masterXfer);

		while ((!MPU6050_nakFlag) && (!MPU6050_completionFlag)){}
		MPU6050_nakFlag = false;
	}

	MPU6050_completionFlag = false;
	if (who_am_i_value == MPU6050_WHOAMI_VALUE_0)
	{
		return true;
	}
	else if (who_am_i_value == MPU6050_WHOAMI_VALUE_1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*********************************************************************************************
 * @brief Configure MPU6050 module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Configure_Device(void)
{
	uint8_t databyte  = 0;
	uint8_t write_reg = 0;

	// Configuration register
	write_reg = MPU6050_CONFIG;
	databyte  = 0;
	MPU6050_WriteAccelReg(I2C1, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);

	// Power mode and clock source
	write_reg = MPU6050_PWR_MGMT_1;
	databyte  = 0;
	MPU6050_WriteAccelReg(I2C1, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);

	// Gyro - +-250°/s
	write_reg = MPU6050_GYRO_CONFIG;
	databyte  = 0;
	MPU6050_WriteAccelReg(I2C1, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);

	// Accel - +-2g
	write_reg = MPU6050_ACCEL_CONFIG;
	databyte  = 0;
	MPU6050_WriteAccelReg(I2C1, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);
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
bool MPU6050_WriteAccelReg(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value)
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

	I2C_MasterTransferNonBlocking(I2C1, &mpu_g_m_handle, &masterXfer);

	/*  wait for transfer completed. */
	while ((!MPU6050_nakFlag) && (!MPU6050_completionFlag))
	{
	}

	MPU6050_nakFlag = false;

	if (MPU6050_completionFlag == true)
	{
		MPU6050_completionFlag = false;
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
bool MPU6050_ReadAccelRegs(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
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

    I2C_MasterTransferNonBlocking(I2C1, &mpu_g_m_handle, &masterXfer);

    /*  wait for transfer completed. */
    while ((!MPU6050_nakFlag) && (!MPU6050_completionFlag))
    {
    }

    MPU6050_nakFlag = false;

    if (MPU6050_completionFlag == true)
    {
        MPU6050_completionFlag = false;
        return true;
    }
    else
    {
        return false;
    }
}



/*********************************************************************************************
 * @brief Read accelerometer data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Read_Accel_Data(I2C_Type *base, uint8_t device_addr, int16_t *xyz_accel)
{
	uint8_t readBuff[6];
	MPU6050_ReadAccelRegs(I2C1, MPU6050_DEVICE_ADDRESS_0, MPU6050_ACCEL_XOUT_H, readBuff, 6);
	xyz_accel[0] = (((int16_t)readBuff[0]) << 8) | readBuff[1];
	xyz_accel[1] = (((int16_t)readBuff[2]) << 8) | readBuff[3];
	xyz_accel[2] = (((int16_t)readBuff[4]) << 8) | readBuff[5];
}

/*********************************************************************************************
 * @brief Read gyroscope data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Read_Gyro_Data(I2C_Type *base, uint8_t device_addr, int16_t *xyz_gyro)
{
	uint8_t readBuff[6];
	MPU6050_ReadAccelRegs(I2C1, MPU6050_DEVICE_ADDRESS_0, MPU6050_GYRO_XOUT_H, readBuff, 6);
	xyz_gyro[0] = (((int16_t)readBuff[0]) << 8) | readBuff[1];
	xyz_gyro[1] = (((int16_t)readBuff[2]) << 8) | readBuff[3];
	xyz_gyro[2] = (((int16_t)readBuff[4]) << 8) | readBuff[5];
}




/*********************************************************************************************
 * @brief Get the angular velocity from Gyro
 *
 * @param pointer to angular velocity vector -- xyz values
 *
 * @return void
 *********************************************************************************************/
void MPU6050_GetAngularVelocity(float *omega)
{
	int16_t xyz_gyro[3];
	MPU6050_Read_Gyro_Data(I2C1, MPU6050_DEVICE_ADDRESS_0, xyz_gyro);
	omega[0] = (float)xyz_gyro[0] / MPU6050_GYRO_FACTOR + 2.5287;
	omega[1] = (float)xyz_gyro[1] / MPU6050_GYRO_FACTOR - 1.13;
	omega[2] = (float)xyz_gyro[2] / MPU6050_GYRO_FACTOR - 0.3583;
}


/*********************************************************************************************
 * @brief Get the Acceleration in g units
 *
 * @param pointer to acceleration vector -- xyz values
 *
 * @return void
 *********************************************************************************************/
void MPU6050_GetgAcceleration(float *accel)
{
	int16_t xyz_accel[3];
	MPU6050_Read_Accel_Data(I2C1, MPU6050_DEVICE_ADDRESS_0, xyz_accel);
	accel[0] = (float)xyz_accel[0] / MPU6050_ACCEL_FACTOR;
	accel[1] = (float)xyz_accel[1] / MPU6050_ACCEL_FACTOR;
	accel[2] = (float)xyz_accel[2] / MPU6050_ACCEL_FACTOR;
}

/*********************************************************************************************
 * @brief Get the X angle from accelerometer
 *
 * @param void
 *
 * @return roll angle
 *********************************************************************************************/
float MPU6050_GetXAngle(void)
{
	int16_t xyz_accel[3];
	MPU6050_Read_Accel_Data(I2C1, MPU6050_DEVICE_ADDRESS_0, xyz_accel);

	float sum_of_squares = powf((float)xyz_accel[0], 2) + powf((float)xyz_accel[2], 2);
	float root = sqrtf(sum_of_squares);
	float Xangle = atanf(xyz_accel[1] / root) * 57.2957;

	return Xangle;
}


/*********************************************************************************************
 * @brief Get the Y angle from accelerometer
 *
 * @param void
 *
 * @return pitch angle
 *********************************************************************************************/
float MPU6050_GetYAngle(void)
{
	int16_t xyz_accel[3];
	MPU6050_Read_Accel_Data(I2C1, MPU6050_DEVICE_ADDRESS_0, xyz_accel);
	float sum_of_squares = powf((float)xyz_accel[1], 2) + powf((float)xyz_accel[2], 2);
	float root = sqrtf(sum_of_squares);
	float Yangle = atanf(xyz_accel[0] / root) * 57.2957;

	return Yangle;
}


/*********************************************************************************************
 * @brief Get X and Y angles with complementary filter
 *
 * @param previous X angle
 * @param previous Y angle
 * @param differential time between measures
 * @param new X angle pointer
 * @param new y angle pointer
 *
 * @return void
 *********************************************************************************************/
void MPU6050_ComplementaryFilterAngles(float x_prev, float y_prev, float dt, float *x_new, float *y_new)
{
	float x_angle = MPU6050_GetXAngle();
	float y_angle = MPU6050_GetYAngle();

	float omega[3];

	MPU6050_GetAngularVelocity(omega);

	*x_new = 0.8 * (x_prev + omega[0] * dt) + 0.2 * x_angle;
	*y_new = 0.8 * (y_prev + omega[1] * dt) + 0.2 * y_angle;
}




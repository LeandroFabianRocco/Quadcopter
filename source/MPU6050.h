/*********************************************************
 * MPU6050.h
 *
 *  Created on: 22 ene. 2020
 *      Author: leandro
 *********************************************************/

#ifndef MPU6050_H_
#define MPU6050_H_

#include "fsl_i2c.h"
#include "math.h"

/**********************************************************
 * MPU6050 registers
 *********************************************************/
// Sensitivity scale factor
#define MPU6050_ACCEL_FACTOR		16384.0
#define MPU6050_GYRO_FACTOR			131.0
// MPU6050 address - Depending of DA0 pin logic
#define MPU6050_DEVICE_ADDRESS_0 	0x68
#define MPU6050_DEVICE_ADDRESS_1 	0x69
// Spected value for WHOAMI register
#define MPU6050_WHOAMI_VALUE_0 		0x68
#define MPU6050_WHOAMI_VALUE_1 		0x69
//
#define MPU6050_SELF_TEST_X			0x0D
#define MPU6050_SELF_TEST_Y 		0x0E
#define MPU6050_SELF_TEST_Z 		0x0F
#define MPU6050_SELF_TEST_A 		0x10
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG 		0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_FIFO_EN				0x23
#define MPU6050_I2C_MST_CTRL		0x24
#define MPU6050_I2C_SLV0_ADDR		0x25
#define MPU6050_I2C_SLV0_REG		0x26
#define MPU6050_I2C_SLV0_CTRL 		0x27
#define MPU6050_I2C_SLV1_ADDR		0x28
#define MPU6050_I2C_SLV1_REG		0x29
#define MPU6050_I2C_SLV1_CTRL		0x2A
#define MPU6050_I2C_SLV2_ADDR		0x2B
#define MPU6050_I2C_SLV2_REG		0x2C
#define MPU6050_I2C_SLV2_CTRL		0x2D
#define MPU6050_I2C_SLV3_ADDR		0x2E
#define MPU6050_I2C_SLV3_REG		0x2F
#define MPU6050_I2C_SLV3_CTRL		0x30
#define MPU6050_I2C_SLV4_ADDR		0x31
#define MPU6050_I2C_SLV4_REG		0x32
#define MPU6050_I2C_SLV4_DO			0x33
#define MPU6050_I2C_SLV4_CTRL		0x34
#define MPU6050_I2C_SLV4_DI			0x35
#define MPU6050_I2C_MST_STATUS		0x36
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_EXT_SENS_DATA_00	0x49
#define MPU6050_EXT_SENS_DATA_01	0x4A
#define MPU6050_EXT_SENS_DATA_02	0x4B
#define MPU6050_EXT_SENS_DATA_03	0x4C
#define MPU6050_EXT_SENS_DATA_04	0x4D
#define MPU6050_EXT_SENS_DATA_05	0x4E
#define MPU6050_EXT_SENS_DATA_06	0x4F
#define MPU6050_EXT_SENS_DATA_07	0x50
#define MPU6050_EXT_SENS_DATA_08	0x51
#define MPU6050_EXT_SENS_DATA_09	0x52
#define MPU6050_EXT_SENS_DATA_10	0x53
#define MPU6050_EXT_SENS_DATA_11	0x54
#define MPU6050_EXT_SENS_DATA_12	0x55
#define MPU6050_EXT_SENS_DATA_13	0x56
#define MPU6050_EXT_SENS_DATA_14	0x57
#define MPU6050_EXT_SENS_DATA_15	0x58
#define MPU6050_EXT_SENS_DATA_16	0x59
#define MPU6050_EXT_SENS_DATA_17	0x5A
#define MPU6050_EXT_SENS_DATA_18	0x5B
#define MPU6050_EXT_SENS_DATA_19	0x5C
#define MPU6050_EXT_SENS_DATA_20	0x5D
#define MPU6050_EXT_SENS_DATA_21	0x5E
#define MPU6050_EXT_SENS_DATA_22	0x5F
#define MPU6050_EXT_SENS_DATA_23	0x60
#define MPU6050_I2C_SLV0_DO			0x63
#define MPU6050_I2C_SLV1_DO			0x64
#define MPU6050_I2C_SLV2_DO			0x65
#define MPU6050_I2C_SLV3_DO			0x66
#define MPU6050_I2C_MST_DELAY_CTRL	0x67
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

/*********************************************************************************************
 * @brief Gets the WHO_AM_I value
 *
 * @param void
 *
 * @return status flag. Return true if no error
 *********************************************************************************************/
bool MPU6050_ReadSensorWhoAmI(void);

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
void MPU6050_i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData);


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
bool MPU6050_WriteAccelReg(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value);

/*********************************************************************************************
 * @brief Initialize MPU6050 module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Init(void);


/*********************************************************************************************
 * @brief Configure MPU6050 module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Configure_Device(void);

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
bool MPU6050_ReadAccelRegs(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);


/*********************************************************************************************
 * @brief Read accelerometer data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Read_Accel_Data(I2C_Type *base, uint8_t device_addr, int16_t *xyz_accel);

/*********************************************************************************************
 * @brief Read gyroscope data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Read_Gyro_Data(I2C_Type *base, uint8_t device_addr, int16_t *xyz_gyro);

/*********************************************************************************************
 * @brief Get the angular velocity from Gyro
 *
 * @param pointer to angular velocity vector -- xyz values
 *
 * @return void
 *********************************************************************************************/
void MPU6050_GetAngularVelocity(float *omega);


/*********************************************************************************************
 * @brief Get the Acceleration in g units
 *
 * @param pointer to acceleration vector -- xyz values
 *
 * @return void
 *********************************************************************************************/
void MPU6050_GetgAcceleration(float *accel);


/*********************************************************************************************
 * @brief Get the X angle from accelerometer
 *
 * @param void
 *
 * @return roll angle
 *********************************************************************************************/
float MPU6050_GetXAngle(void);


/*********************************************************************************************
 * @brief Get the Y angle from accelerometer
 *
 * @param void
 *
 * @return pitch angle
 *********************************************************************************************/
float MPU6050_GetYAngle(void);


/*********************************************************************************************
 * @brief Get X and Y angles with complementary filter
 *
 * @param previous X angle
 * @param previous Y angle
 * @param diferential time between measures
 * @param new X angle pointer
 * @param new y angle pointer
 *
 * @return void
 *********************************************************************************************/
void MPU6050_ComplementaryFilterAngles(float x_prev, float y_prev, float dt, float *x_new, float *y_new);


#endif /* MPU6050_H_ */


#ifndef _I2C_WRAPPER_H
#define _I2C_WRAPPER_H



///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    I2C_OK = 0,
    I2C_ALREADY_INIT = -1,
    I2C_WRONG_PARAMETER = -2,
    I2C_DEVICE_NOT_AVAILABLE = -3,
    I2C_MUTEX_ERROR = -4,
    I2C_TRANSFER_ERROR = -5
}i2c_status_t;

typedef struct
{
    int32_t I2cFileHandler;
    pthread_mutex_t I2cMutex;
}i2c_handler_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function-like Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Initializes the i2c driver
 *
 * @param Device Pointer to the I2C device
 * @param Handler Pointer to the I2C handler
 * @return I2C_OK on success, or error.
 */
i2c_status_t I2c_Init(uint8_t* Device, i2c_handler_t * Handler);
/*!
 * @brief Initializes the i2c driver
 *
 * @param Handler Pointer to the I2C handler
 * @return void.
 */
void I2c_Deinit(i2c_handler_t * Handler);
/*!
 * @brief Writes the specified data to the selected I2C slave
 *
 * @param Handler Pointer to the I2C handler
 * @param Target slave address
 * @param Buffer to be sent
 * @param Amount of bytes to send
 * @return I2C_OK on success, or error.
 */

i2c_status_t I2c_Write(i2c_handler_t * Handler, uint32_t SlaveAddress, \
        uint8_t *DataBuffer, size_t DataLength);
 /*!
 * @brief Reads from the from the specified location
 *
 * @param Handler Pointer to the I2C handler
 * @param Target slave address
 * @param Buffer to be sent. This should hold the start read address
 * @param Amount of bytes to send
 * @param Buffer to store received data
 * qparam Amount of data to receive
 * @return I2C_OK on success, or error.
 */
       
i2c_status_t I2c_Read(i2c_handler_t * Handler, uint32_t SlaveAddress, \
        uint8_t *DataOutBuffer, size_t DataOutLength, uint8_t * DataInBuffer, \
            size_t DataInLength);
 
#endif /* _I2C_WRAPPER_H */


/* EOF */

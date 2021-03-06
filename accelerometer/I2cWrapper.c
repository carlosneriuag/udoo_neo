///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <pthread.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include "I2cWrapper.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

/*http://bunniestudios.com/blog/images/infocast_i2c.c*/ 

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

i2c_status_t I2c_Init(uint8_t* Device, i2c_handler_t * Handler)
{
    static bool isInitialized = false;
    i2c_status_t Status = I2C_OK;
    int32_t MutexStatus;
    
    if(isInitialized == false)
    {
        if((Handler != NULL) && (Device != NULL))
        {
            Handler->I2cFileHandler = open((char*)Device,O_RDWR);
            
            if(Handler->I2cFileHandler > 0)
            {
                /* once the device is opened, create a mutex */
                /* so this device can be shared */                
                MutexStatus = pthread_mutex_init(&Handler->I2cMutex,NULL);
                
                if(MutexStatus)
                {
                    Status = I2C_MUTEX_ERROR;                
                }
                else
                {
                    isInitialized = true;
                }
                
            }
            else
            {
                Status = I2C_DEVICE_NOT_AVAILABLE;
            }
        }
        else
        {
            Status = I2C_WRONG_PARAMETER;
        }
    }
    else
    {
        Status = I2C_ALREADY_INIT;
    }

    return (Status);
}


i2c_status_t I2c_Write(i2c_handler_t * Handler, uint32_t SlaveAddress, \
        uint8_t *DataBuffer, size_t DataLength)
{
    
	i2c_status_t Status = I2C_OK;
    int32_t I2cTransferStatus;
	struct i2c_rdwr_ioctl_data I2cMsgQueue;
    struct i2c_msg DataMsg;
    
    
    if((Handler != NULL) && (DataBuffer != NULL))
    {
        /* make sure to take the resource when is available */
        pthread_mutex_lock(&Handler->I2cMutex);
        
        I2cMsgQueue.nmsgs = 1;
		I2cMsgQueue.msgs = &DataMsg;
        
        /* fill the message with the desired data */        
        DataMsg.len = DataLength;
		DataMsg.addr = SlaveAddress;
		DataMsg.buf = DataBuffer;
		DataMsg.flags = 0;
        
	/* execute the write command */
        I2cTransferStatus = ioctl(Handler->I2cFileHandler, I2C_RDWR, &I2cMsgQueue);

        if(I2cTransferStatus < 0)
	{
		Status = I2C_TRANSFER_ERROR;
	}
        
    }
    else
    {
        Status = I2C_WRONG_PARAMETER;
    }
 
    pthread_mutex_unlock(&Handler->I2cMutex);
    
    return (Status);
}


i2c_status_t I2c_Read(i2c_handler_t * Handler, uint32_t SlaveAddress, \
        uint8_t *DataOutBuffer, size_t DataOutLength, uint8_t * DataInBuffer, \
            size_t DataInLength)
{
    
    i2c_status_t Status = I2C_OK;
    int32_t I2cTransferStatus;
	struct i2c_rdwr_ioctl_data I2cMsgQueue;
    struct i2c_msg DataMsg[2];
    
    if((Handler != NULL) && (DataOutBuffer != NULL) && (DataInBuffer != NULL))
    {
        /* make sure to take the resource when is available */
        pthread_mutex_lock(&Handler->I2cMutex);
        
        I2cMsgQueue.nmsgs = 2;
		I2cMsgQueue.msgs = &DataMsg[0];
        
	/* first, write the register address */

        /* fill the message with the desired data */        
        DataMsg[0].len = DataOutLength;
		DataMsg[0].addr = SlaveAddress;
		DataMsg[0].buf = DataOutBuffer;
		DataMsg[0].flags = 0;
        /* data input information */
        DataMsg[1].len = DataInLength;
        DataMsg[1].addr = SlaveAddress;
        DataMsg[1].buf = DataInBuffer;
        DataMsg[1].flags = I2C_M_RD;
        
        I2cTransferStatus = ioctl(Handler->I2cFileHandler, I2C_RDWR, &I2cMsgQueue);

        if(I2cTransferStatus <  0)
		{
                Status = I2C_TRANSFER_ERROR;    
		}
    }
    else
    {
        Status = I2C_WRONG_PARAMETER;
    }
    
    pthread_mutex_unlock(&Handler->I2cMutex);
    
    return (Status);
}

void I2c_Deinit(i2c_handler_t * Handler)
{
    if(Handler != NULL)
    {
        close(Handler->I2cFileHandler);
        
        pthread_mutex_destroy(&Handler->I2cMutex);
    }
}
/* EOF */

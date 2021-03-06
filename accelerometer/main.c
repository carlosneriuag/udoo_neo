///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2cWrapper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define ACCELEROMETER_ADDRESS 			0x1E

#define WHO_AM_I_REGISTER 				0x0D

#define ACCELEROMETER_BUFFER_SIZE		6

#define ACCELEROMETER_CONTROL_REGISTER		0x2B

#define ACCELEROMETER_X_DATA_MSB_REGISTER	0x01

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensor_PrintSignedNumber(int16_t DataToDisplay);

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

int main (int argc, char * argv[])
{

	i2c_status_t I2cStatus;
	i2c_handler_t I2cHandler;	
	uint8_t * AccelerometerReadBuffer;
	uint8_t * AccelerometerWriteBuffer;
	int16_t AxisX;
	int16_t AxisY;
	int16_t AxisZ;


	if(argc >= 2)
	{
           
		I2cStatus = I2c_Init((uint8_t*)argv[1],&I2cHandler);

		if(I2cStatus == I2C_OK)
		{
			AccelerometerReadBuffer = (uint8_t*)malloc(ACCELEROMETER_BUFFER_SIZE);
			AccelerometerWriteBuffer = (uint8_t*)malloc(ACCELEROMETER_BUFFER_SIZE);
            
            if((AccelerometerReadBuffer != NULL) && (AccelerometerWriteBuffer != NULL))
            {
                /* Assume the buffers were allocated */
                AccelerometerWriteBuffer[0] = WHO_AM_I_REGISTER;

                I2cStatus = I2c_Read(&I2cHandler, ACCELEROMETER_ADDRESS, AccelerometerWriteBuffer,\
                                1, AccelerometerReadBuffer, 1);
                                
                                
                if((I2cStatus == I2C_OK) && (AccelerometerReadBuffer[0] == 0xC7))
                {
                    printf("FXOS8700CQ Found!\n\r");

                    /* Enable accelerometer */
                    AccelerometerWriteBuffer[0] = ACCELEROMETER_CONTROL_REGISTER;
                    AccelerometerWriteBuffer[1] = 0x01;

                    I2cStatus = I2c_Write(&I2cHandler, ACCELEROMETER_ADDRESS, AccelerometerWriteBuffer,2);
                    if(I2cStatus == I2C_OK)
                    {

                        AccelerometerWriteBuffer[0] = ACCELEROMETER_X_DATA_MSB_REGISTER;

                        printf("\n\rPress enter for a new reading...");
                        while(1)
                        {
                            fgets((char*)&AccelerometerWriteBuffer[4],2,stdin);

                            I2cStatus = I2c_Read(&I2cHandler, ACCELEROMETER_ADDRESS, AccelerometerWriteBuffer,\
                                    1, AccelerometerReadBuffer, 6);
                            
                            if(I2cStatus != I2C_OK)
                            {
                                printf("\n\rError Reading Accelerometer\n\r");
                            }
                            
                            /* Accelerometer data is 14-bit 2's Complement left justified */

                            AxisX = (int16_t)((uint16_t)((uint16_t)AccelerometerReadBuffer[0] << 8) | (uint16_t)AccelerometerReadBuffer[1]) >> 2U;
                            
                            AxisY = (int16_t)((uint16_t)((uint16_t)AccelerometerReadBuffer[2] << 8) | (uint16_t)AccelerometerReadBuffer[3]) >> 2U;
                            
                            AxisZ = (int16_t)((uint16_t)((uint16_t)AccelerometerReadBuffer[4] << 8) | (uint16_t)AccelerometerReadBuffer[5]) >> 2U;    
                            
                            
                            printf("\n\rX Axis: ");
                            Sensor_PrintSignedNumber(AxisX);
                            printf("\n\rY Axis: ");
                            Sensor_PrintSignedNumber(AxisY);
                            printf("\n\rZ Axis: ");
                            Sensor_PrintSignedNumber(AxisZ);                        					
                        }

                    }
                    else
                    {
                        printf("\n\rError writing to I2C Device: %d\n\r",I2cStatus);
                    }
                
                }
            }
            else
            {
                printf("Error while allocating buffers\n\r");
            }
		}
		else
		{
			printf("\n\rError opening I2C Device: %d\n\r",I2cStatus);
		}

	}
	else
	{

		printf("Error Usage\n\rI2cWrapperTest <i2cdevice>\n\r");
	}

	return(0);

}

static void Sensor_PrintSignedNumber(int16_t DataToDisplay)
{
	uint16_t Data = (uint16_t)DataToDisplay;

	/* verify if data is positive or negative */
	/* note this function will work only with 14-bit numbers */
	if(Data < 0x1FFF)
	{
		printf("+");
	}
	else
	{
		printf("-");
		/* convert 2's complement back*/
		Data = ((~Data) + 1);
	}

	printf("%d",Data);
}
/* EOF */

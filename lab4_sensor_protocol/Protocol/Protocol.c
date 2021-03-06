///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "Protocol.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t CalculateChecksum(uint8_t * Frame, uint16_t FrameSize);

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


protocolstatus_t Protocol_BuildCommandFrame(uint8_t ** OutputBuffer, sensors_t Sensor, axis_t Axis)
{
    protocolstatus_t Status = PROTOCOL_SUCCESS;
    uint8_t * NewFrame;
    
    if((Sensor <= SENSOR_GYROSCOPE) || (Sensor == SENSOR_ALL))
    {
        if((Axis <= AXIS_Z) || (Axis == AXIS_ALL))
        {
            NewFrame = (uint8_t *)malloc(PROTOCOL_COMMAND_FRAME_SIZE);
            
            if(NewFrame != NULL)
            {    
                NewFrame[PROTOCOL_COMMAND_SOF_OFFSET] = PROTOCOL_FRAME_SOF;
                
                NewFrame[PROTOCOL_COMMAND_SENSOR_OFFSET] = Sensor;
                 
                NewFrame[PROTOCOL_COMMAND_AXIS_OFFSET] = Axis;
                  
                NewFrame[PROTOCOL_COMMAND_CS_OFFSET] = CalculateChecksum(NewFrame, (PROTOCOL_COMMAND_FRAME_SIZE - 1));
                
                #ifdef DEBUG
                printf("DEBUG %s: Frame created: 0x%X 0x%X 0x%X 0x%X\n\r",__FUNCTION__,NewFrame[PROTOCOL_COMMAND_SOF_OFFSET], NewFrame[PROTOCOL_COMMAND_SENSOR_OFFSET], \
                    NewFrame[PROTOCOL_COMMAND_AXIS_OFFSET], NewFrame[PROTOCOL_COMMAND_CS_OFFSET]);
                #endif
                
                *OutputBuffer = NewFrame;
            }
            else
            {
                Status = PROTOCOL_ERROR;
            }
        }
        else
        {
            Status = PROTOCOL_WRONG_AXIS;
        }      
    }
    else
    {
        Status = PROTOCOL_WRONG_SENSOR;
    }
    
    return (Status);
}

protocolstatus_t Protocol_ParseResponseFrame(uint8_t * InputBuffer, sensorframe_t * ReceivedFrame)
{
    protocolstatus_t Status = PROTOCOL_SUCCESS;
    uint8_t CalculatedChecksum;
    uint8_t ChecksumOffset;
    
    if((InputBuffer != NULL) && (ReceivedFrame != NULL))
    {
        if(InputBuffer[PROTOCOL_RESPONSE_SOF_OFFSET] == PROTOCOL_FRAME_SOF)
        {
            if((InputBuffer[PROTOCOL_RESPONSE_SENSOR_OFFSET] <= SENSOR_GYROSCOPE) || \
                (InputBuffer[PROTOCOL_RESPONSE_SENSOR_OFFSET] == SENSOR_ALL))
            {

                ReceivedFrame->Sensor = InputBuffer[PROTOCOL_RESPONSE_SENSOR_OFFSET];
                ReceivedFrame->DataSize = InputBuffer[PROTOCOL_RESPONSE_DATA_SIZE_OFFSET];
                
                CalculatedChecksum = CalculateChecksum(&InputBuffer[PROTOCOL_RESPONSE_SOF_OFFSET], ReceivedFrame->DataSize + 2);
                
                ChecksumOffset = PROTOCOL_RESPONSE_CS_OFFSET(ReceivedFrame->DataSize);
                
                #ifdef DEBUG
                printf("DEBUG %s: Received Checksum = 0x%X\n\r",__FUNCTION__,InputBuffer[ChecksumOffset]);
                printf("DEBUG %s: Calculated Checksum = 0x%X\n\r",__FUNCTION__,CalculatedChecksum);
                #endif
                
                if(CalculatedChecksum == InputBuffer[ChecksumOffset])
                {
                
                    ReceivedFrame->Payload = malloc(ReceivedFrame->DataSize);
                
                    memcpy(ReceivedFrame->Payload,&InputBuffer[PROTOCOL_RESPONSE_PAYLOAD_OFFSET],ReceivedFrame->DataSize);
                }
                else
                {
                    Status = PROTOCOL_WRONG_CS;
                }
            }
            else if(InputBuffer[PROTOCOL_RESPONSE_SENSOR_OFFSET] == SENSOR_ALL)
            {
                Status = PROTOCOL_ERROR_PREV_FRAME;
            }
            else
            {
                Status = PROTOCOL_ERROR;
            }
        }
        else
        {
            Status = PROTOCOL_WRONG_SOF;
        } 
    }
    
    return (Status);
}

static uint8_t CalculateChecksum(uint8_t * Frame, uint16_t FrameSize)
{
	uint8_t Checksum = 0;
	uint16_t FrameOffset = 0;

	while(FrameSize--)
	{
		Checksum += Frame[FrameOffset];
		FrameOffset++;
	}

	return(~Checksum);

}

/* EOF */
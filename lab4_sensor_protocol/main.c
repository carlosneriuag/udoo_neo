///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include "Shell.h"
#include "Protocol.h"
#include <errno.h>
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define LAB4_SHELL_PROMPT       ("lab4>")

#define GET_ARRAY_SIZE(x)       (sizeof(x)/sizeof(x[0]))

#define MAX_CLIENTS             (20)

#define MQ_MODE       			(S_IRWXU | S_IRWXG | S_IRWXO)

#define MAX_MESSAGES			(5)

#define	MAX_MESSAGES_SIZE		(55)

#define MESSAGE_QUEUE_PATH		(15)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    pthread_t ThreadId;
	int32_t SocketHandler;
	int32_t ClientId;
	mqd_t * MessageQueue;
    char * MessageQueueName;
}sensorclient_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void ShellCommand_WrongSof(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_Accelerometer(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_Magnetometer(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_Gyroscope(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_AllSensors(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_WrongSensor(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_WrongCs(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_WrongAxis(char * CommandParameters[], uint16_t CommandParameterCount);

static void ShellCommand_Exit(char * CommandParameters[], uint16_t CommandParameterCount);

static void * SensorClient(void* parameter);

static void SendFrameToClients(uint8_t *Message);

static void PrintFrame(sensorframe_t * Frame);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static const shellcmd_t ShellCommands[] =
{		
		{"accel", ShellCommand_Accelerometer},
		{"mag", ShellCommand_Magnetometer},
		{"gyro", ShellCommand_Gyroscope},
		{"all", ShellCommand_AllSensors},
		{"wrongsensor", ShellCommand_WrongSensor},
		{"wrongcs", ShellCommand_WrongCs},
		{"wrongaxis", ShellCommand_WrongAxis},
		{"wrongsof", ShellCommand_WrongSof},
        {"exit", ShellCommand_Exit}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static sensorclient_t * ClientHandler;

static uint16_t ClientListSize;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char * argv[])
{
    uint16_t ClientNumber;
    uint32_t ServerPort;
    char * MessageQueueThreadPath;
	char * ThreadIdAscii;
    uint8_t ClientOffset = 0;
    int32_t ClientSocket;
    struct mq_attr QueueAttributes;
    int32_t Status;
    struct sockaddr_in SocketSettings;
    mqd_t * ClientQueue;
    
    if(argc == 4)
	{
        
        ClientNumber = atoi(argv[1]);
        
        if(ClientNumber <= MAX_CLIENTS)
        {
            
            printf("\n\rCreating %d clients\n\r",ClientNumber);
            
            ClientListSize = ClientNumber;
            
            /* Create client threads and connect */           
            ClientHandler = (sensorclient_t*)malloc(ClientNumber*sizeof(sensorclient_t));
            
            if(ClientHandler != NULL)
            {
                                
                ServerPort = atoi(argv[3]);
            
                /* set server settings */
                SocketSettings.sin_family = AF_INET;
                SocketSettings.sin_port = htons(ServerPort);
                SocketSettings.sin_addr.s_addr = inet_addr(argv[2]);
            
                ThreadIdAscii = (char*)malloc(5);
            
                if(ThreadIdAscii != NULL)
                {
                    while(ClientOffset < ClientNumber)
                    {
                        ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
                        
                        /*Try to connect the socket for the communication*/
                        Status = connect(ClientSocket, (struct sockaddr *)&SocketSettings,sizeof(struct sockaddr));
                        
                        if(Status >= 0)
                        {                           
                            MessageQueueThreadPath = (char*)malloc(MESSAGE_QUEUE_PATH);
                            
                            if(MessageQueueThreadPath != NULL)
                            {
                                /* once connected, create the thread and pass its own message queue */
                                QueueAttributes.mq_maxmsg = MAX_MESSAGES;
                                QueueAttributes.mq_msgsize = MAX_MESSAGES_SIZE;
                                QueueAttributes.mq_flags = 0;
                                
                                strcpy(MessageQueueThreadPath,"/sensorq");

                                sprintf((char*)ThreadIdAscii, "%d", ClientOffset);

                                MessageQueueThreadPath = strcat((char*)MessageQueueThreadPath,(char*)ThreadIdAscii);

                                ClientQueue = (mqd_t *)malloc(sizeof(mqd_t));

        
                                if(ClientQueue != NULL)
                                {
                                                                        
                                    *ClientQueue  = mq_open((char*)MessageQueueThreadPath, O_CREAT|O_RDWR, 0664,&QueueAttributes);
                                    
                                    if(*ClientQueue > 0)
                                    {
                                        ClientHandler[ClientOffset].MessageQueue = ClientQueue;
                                        
                                        ClientHandler[ClientOffset].MessageQueueName = MessageQueueThreadPath;

                                        ClientHandler[ClientOffset].SocketHandler = ClientSocket;

                                        ClientHandler[ClientOffset].ClientId = ClientOffset;
                                        
                                        #ifdef DEBUG
                                        printf("DEBUG %s:ClientHandler[%d].MessageQueue = %d\n\r"
                                            "\t\t\t   .MessageQueueName = %s\n\r",__FUNCTION__,ClientOffset,*ClientHandler[ClientOffset].MessageQueue,\
                                                ClientHandler[ClientOffset].MessageQueueName);
                                        #endif

                                        /* Connect the client, create the thread with its mq */
                                        Status = pthread_create(&ClientHandler[ClientOffset].ThreadId,NULL,SensorClient,&ClientHandler[ClientOffset]);
                                        
                                        ClientOffset++;
                                        /* sleep for 10ms */
                                        usleep(10000);
                                    }
                                    else
                                    {
                                        perror("Main");
                                    }
                                }
                                else
                                {
                                    printf("\n\rError creating MQ %d\n\r",ClientOffset);
                                }
                            }
                            else
                            {
                                printf("\n\rErrir while allocating buffer");
                            }
                        }
                        else
                        {
                            printf("\n\rFailed to connect to server\n\r");
                            break;
                        }
                    }
                    
                    free(ThreadIdAscii);
                    
                    /* keep running only if all threads were created */
                    if(ClientOffset == ClientNumber)
                    {
                        /* Shell thread will handle user interaction */
                        Shell_Initialization((shellcmd_t*)&ShellCommands[0],GET_ARRAY_SIZE(ShellCommands),LAB4_SHELL_PROMPT);

                        while(1)
                        {
                        } 
                    }
                }
                else
                {
                    printf("Max number of clients is %d\n\r", MAX_CLIENTS);
                }
                

            }
        }
        else
        {
            printf("Max number of clients is %d\n\r", MAX_CLIENTS);
        }
    }
    else
    {
        printf("Usage: ./lab4_sensor_protocol.out <clients> <serverip> <serverport>\nclients = number of clients to create\n\r"
                "serverip = remote server IP\nport = remote server listening port\n\r");
    }
    
	return (0);

}


void ShellCommand_Accelerometer(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t AxisToUse;
    axis_t Axis;
    
    if(CommandParameterCount == 1)
    {
        AxisToUse = atoi(CommandParameters[0]);
        
        if((AxisToUse >= 0) && (AxisToUse <= 3))
        {
            if((AxisToUse >= 0) && (AxisToUse <= 2))
            {
                Axis = AxisToUse + 1;
            }
            else
            {
                Axis = AXIS_ALL;
            }
            
            Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_ACCELEROMETER,Axis);
            
            if(Status == PROTOCOL_SUCCESS)
            {
                SendFrameToClients(FrameToSend); 
            }
            else
            {
                printf("Error while building frame %d\n\r",Status);
            }           
        }
        else 
        {
           printf("Usage: accel <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r"); 
        }
    }
    else
    {
        printf("Usage: accel <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r");
    }
}

void ShellCommand_Magnetometer(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t AxisToUse;
    axis_t Axis;
    
    if(CommandParameterCount == 1)
    {
        AxisToUse = atoi(CommandParameters[0]);
        
        if((AxisToUse >= 0) && (AxisToUse <= 3))
        {
            if((AxisToUse >= 0) && (AxisToUse <= 2))
            {
                Axis = AxisToUse + 1;
            }
            else
            {
                Axis = AXIS_ALL;
            }
            
            Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_MAGNETOMETER,Axis);
            
            if(Status == PROTOCOL_SUCCESS)
            {
                SendFrameToClients(FrameToSend); 
            }
            else
            {
                printf("Error while building frame %d\n\r",Status);
            }           
        }
        else 
        {
           printf("Usage: mag <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r"); 
        }
    }
    else
    {
        printf("Usage: mag <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r");
    }
}

void ShellCommand_Gyroscope(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t AxisToUse;
    axis_t Axis;
    
    if(CommandParameterCount == 1)
    {
        AxisToUse = atoi(CommandParameters[0]);
        
        if((AxisToUse >= 0) && (AxisToUse <= 3))
        {
            if((AxisToUse >= 0) && (AxisToUse <= 2))
            {
                Axis = AxisToUse + 1;
            }
            else
            {
                Axis = AXIS_ALL;
            }
            
            Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_GYROSCOPE,Axis);
            
            if(Status == PROTOCOL_SUCCESS)
            {
                SendFrameToClients(FrameToSend); 
            }
            else
            {
                printf("Error while building frame %d\n\r",Status);
            }           
        }
        else 
        {
           printf("Usage: gyro <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r"); 
        }
    }
    else
    {
        printf("Usage: gyro <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r");
    }
}

void ShellCommand_AllSensors(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t AxisToUse;
    axis_t Axis;
    
    if(CommandParameterCount == 1)
    {
        AxisToUse = atoi(CommandParameters[0]);
        
        if((AxisToUse >= 0) && (AxisToUse <= 3))
        {
            if((AxisToUse >= 0) && (AxisToUse <= 2))
            {
                Axis = AxisToUse + 1;
            }
            else
            {
                Axis = AXIS_ALL;
            }
            
            Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_ALL,Axis);
            
            if(Status == PROTOCOL_SUCCESS)
            {
                SendFrameToClients(FrameToSend); 
            }
            else
            {
                printf("Error while building frame %d\n\r",Status);
            }           
        }
        else 
        {
           printf("Usage: all <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r"); 
        }
    }
    else
    {
        printf("Usage: all <axis>\n\r"
                "axis = 0 - x, 1 - y, 2 - z, 3 - xyz\n\r");
    }
}

void ShellCommand_WrongSensor(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t RandomSensor;
    
    /* Create a valid frame, modify it afterwards */
    Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_ALL,AXIS_ALL);
    
    if(Status == PROTOCOL_SUCCESS)
    {
        /* Make sure the random sensor is not a valid one */
        do
        {
            RandomSensor = rand();
            RandomSensor &= 0xFF;
        }while((RandomSensor <= SENSOR_GYROSCOPE) || (RandomSensor == SENSOR_ALL));
        
        FrameToSend[PROTOCOL_COMMAND_SENSOR_OFFSET] = RandomSensor;
        
        #ifdef DEBUG
        printf("DEBUG %s: Frame 0x%X 0x%X 0x%X 0x%X\n\r",__FUNCTION__,FrameToSend[PROTOCOL_COMMAND_SOF_OFFSET], FrameToSend[PROTOCOL_COMMAND_SENSOR_OFFSET], \
        FrameToSend[PROTOCOL_COMMAND_AXIS_OFFSET], FrameToSend[PROTOCOL_COMMAND_CS_OFFSET]);
        #endif
        
        SendFrameToClients(FrameToSend); 
    }
    else
    {
        printf("Error while building frame %d\n\r",Status);
    }           
}

void ShellCommand_WrongCs(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t RandomCs;
    
    /* Create a valid frame, modify it afterwards */
    Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_ALL,AXIS_ALL);
    
    if(Status == PROTOCOL_SUCCESS)
    {
        /* Make sure the random CS doesn't match the current one */
        do
        {
            RandomCs = rand();
            RandomCs &= 0xFF;
        }while(RandomCs == FrameToSend[PROTOCOL_COMMAND_CS_OFFSET]);
        
        FrameToSend[PROTOCOL_COMMAND_CS_OFFSET] = RandomCs;
        
        #ifdef DEBUG
        printf("DEBUG %s: Frame 0x%X 0x%X 0x%X 0x%X\n\r",__FUNCTION__,FrameToSend[PROTOCOL_COMMAND_SOF_OFFSET], FrameToSend[PROTOCOL_COMMAND_SENSOR_OFFSET], \
        FrameToSend[PROTOCOL_COMMAND_AXIS_OFFSET], FrameToSend[PROTOCOL_COMMAND_CS_OFFSET]);
        #endif
        
        SendFrameToClients(FrameToSend); 
    }
    else
    {
        printf("Error while building frame %d\n\r",Status);
    } 
}

void ShellCommand_WrongAxis(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t RandomAxis;
    
    /* Create a valid frame, modify it afterwards */
    Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_ALL,AXIS_ALL);
    
    if(Status == PROTOCOL_SUCCESS)
    {
        /* Make sure the random axis is not a valid one */
        do
        {
            RandomAxis = rand();
            RandomAxis &= 0xFF;
        }while(RandomAxis <= AXIS_ALL);
        
        FrameToSend[PROTOCOL_COMMAND_AXIS_OFFSET] = RandomAxis;
        
        #ifdef DEBUG
        printf("DEBUG %s: Frame 0x%X 0x%X 0x%X 0x%X\n\r",__FUNCTION__,FrameToSend[PROTOCOL_COMMAND_SOF_OFFSET], FrameToSend[PROTOCOL_COMMAND_SENSOR_OFFSET], \
        FrameToSend[PROTOCOL_COMMAND_AXIS_OFFSET], FrameToSend[PROTOCOL_COMMAND_CS_OFFSET]);
        #endif

        SendFrameToClients(FrameToSend); 
    }
    else
    {
        printf("Error while building frame %d\n\r",Status);
    }           
}

void ShellCommand_WrongSof(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t * FrameToSend = NULL;
    protocolstatus_t Status;
    int32_t RandomSof;
    
    /* Create a valid frame, modify it afterwards */
    Status = Protocol_BuildCommandFrame(&FrameToSend,SENSOR_ALL,AXIS_ALL);
    
    if(Status == PROTOCOL_SUCCESS)
    {
        /* Make sure the random SOF is not a valid one */
        do
        {
            RandomSof = rand();
            RandomSof &= 0xFF;
        }while(RandomSof == PROTOCOL_FRAME_SOF);
        
        FrameToSend[PROTOCOL_COMMAND_SOF_OFFSET] = RandomSof;
        
        #ifdef DEBUG
        printf("DEBUG %s: Frame 0x%X 0x%X 0x%X 0x%X\n\r",__FUNCTION__,FrameToSend[PROTOCOL_COMMAND_SOF_OFFSET], FrameToSend[PROTOCOL_COMMAND_SENSOR_OFFSET], \
        FrameToSend[PROTOCOL_COMMAND_AXIS_OFFSET], FrameToSend[PROTOCOL_COMMAND_CS_OFFSET]);
        #endif
        
        SendFrameToClients(FrameToSend); 
    }
    else
    {
        printf("Error while building frame %d\n\r",Status);
    } 
}

static void ShellCommand_Exit(char * CommandParameters[], uint16_t CommandParameterCount)
{
    uint8_t ClientOffset = 0;
    int32_t MqStatus;
    
    (void)CommandParameters;
    (void)CommandParameterCount;
    
    while(ClientOffset < ClientListSize)
    {
        
        #ifdef DEBUG
        printf("DEBUG %s: Closing Client's %d MQ %d\n\r",__FUNCTION__,ClientOffset,*ClientHandler[ClientOffset].MessageQueue);
        #endif
        
        MqStatus = mq_close(*ClientHandler[ClientOffset].MessageQueue);
        
        if(MqStatus >= 0)
        {
            #ifdef DEBUG
            printf("DEBUG %s: Unlinking client's %d MQ %s\n\r",__FUNCTION__,ClientOffset,ClientHandler[ClientOffset].MessageQueueName);
            #endif
            
            MqStatus = mq_unlink(ClientHandler[ClientOffset].MessageQueueName); 

            if(MqStatus >= 0)
            {
                free(ClientHandler[ClientOffset].MessageQueue);
                free(ClientHandler[ClientOffset].MessageQueueName);
            }
            else
            {
                perror("ShellCommand_Exit Mq Unlink");
                break;
            }
        }
        else
        {
            perror("ShellCommand_Exit Mq Close");
            break;
        }
        
        ClientOffset++;
    }
    
    free(ClientHandler);
    
    exit(0);
}

static void SendFrameToClients(uint8_t *Message)
{
	uint32_t Clients = 0;
    int32_t SendStatus;
    
	while (Clients < ClientListSize)
	{
        #ifdef DEBUG
        printf("DEBUG %s: Sending message to clients %d at MQ %d\n\r",__FUNCTION__,Clients,*ClientHandler[Clients].MessageQueue);
        #endif
		SendStatus = mq_send(*ClientHandler[Clients].MessageQueue,(char*)Message,PROTOCOL_COMMAND_FRAME_SIZE,0);

        if(!SendStatus)
        {
            Clients++;
        }
        else
        {
            perror("SendFrameToClients");
            break;
        }
        
        usleep(1000);
	}
    
    free(Message);
}

static void PrintFrame(sensorframe_t * Frame)
{
    uint8_t PayloadOffset = 0;
    
    printf("\n\rReceived Frame: \n\r");
    printf("\tSensor: ");
    
    switch(Frame->Sensor)
    {
        case SENSOR_ACCELEROMETER:
        {
            printf("Accelerometer\n\r");
        }
        break;
        
        case SENSOR_MAGNETOMETER:
        {
            printf("Accelerometer\n\r");
        }
        break;
        
        case SENSOR_GYROSCOPE:
        {
            printf("Accelerometer\n\r");
        }
        break;
        
        case SENSOR_ALL:
        {
            printf("Accelerometer\n\r");
        }
        break;
        
        default:
        {
            printf("Unexpected Sensor\n\r");
        }
        break;
    }
    
    printf("\tPayload: ");
    
    while(PayloadOffset < Frame->DataSize)
    {
        printf("0x%X ",Frame->Payload[PayloadOffset]);
        
        PayloadOffset++;
    }
    
    printf("\n\r");
}

static void * SensorClient(void* parameter)
{
    ssize_t DataReceived;
    sensorclient_t * SensorHandler = (sensorclient_t*)(parameter);
    uint8_t * MessageBuffer = NULL;
    uint8_t * ServerBuffer = NULL;
    sensorframe_t * ReceivedFrame;
    protocolstatus_t FrameStatus;
    #ifdef DEBUG
    uint8_t DataOffset = 0;
    #endif
    
    MessageBuffer = (uint8_t*)malloc(MAX_MESSAGES_SIZE);
    ServerBuffer = (uint8_t*)malloc(MAX_MESSAGES_SIZE);
    
    #ifdef DEBUG
    printf("DEBUG %s: Client ID: %d\t MQ ID: %d\n\r",__FUNCTION__,SensorHandler->ClientId,*SensorHandler->MessageQueue);
    #endif
    
    if((MessageBuffer != NULL) && (ServerBuffer != NULL))
    {
    
        while(1)
        {
            DataReceived = mq_receive(*SensorHandler->MessageQueue,(char*)MessageBuffer,MAX_MESSAGES_SIZE,NULL);
            #ifdef DEBUG
            printf("DEBUG %s %d: Received a message of size %ld\n\r",__FUNCTION__,SensorHandler->ClientId,DataReceived);
            #endif
            if(DataReceived > 0)
            {
                write(SensorHandler->SocketHandler,MessageBuffer,DataReceived);
                
                DataReceived = read(SensorHandler->SocketHandler,ServerBuffer,MAX_MESSAGES_SIZE);
                
                #ifdef DEBUG
                printf("DEBUG %s: Received Data:\n\r\t",__FUNCTION__);
                while(DataOffset < DataReceived)
                {
                    printf("0x%X ", ServerBuffer[DataOffset]);
                    
                    DataOffset++;
                }
                 printf("\n\r");
                #endif
                
                if(DataReceived > 0)
                {
                    ReceivedFrame = (sensorframe_t*)malloc(sizeof(sensorframe_t));
                    
                    if(ReceivedFrame != NULL)
                    {
                        FrameStatus = Protocol_ParseResponseFrame(ServerBuffer, ReceivedFrame);
                        
                        if(FrameStatus == PROTOCOL_SUCCESS)
                        {
                            PrintFrame(ReceivedFrame);
                            
                            free(ReceivedFrame->Payload);
                            free(ReceivedFrame);
                        }
                        else
                        {
                            printf("Error while parsing frame %d\n\r",FrameStatus);
                        }
                    }
                    else
                    {
                        printf("Error while allocating sensorframe_t\n\r");
                    }
                }
            }
            else
            {
                perror("Client");
            }
        }
    }
    else
    {
        printf("Error while allocating buffers\n\r");
    }
    
    pthread_exit(NULL);
}
/* EOF */
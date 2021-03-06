///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mqueue.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include "Shell.h"
#include "Protocol.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_COMMANDS_ALLOWED    9	

#define MQ_MODE       			(S_IRWXU | S_IRWXG | S_IRWXO)

#define MAX_MESSAGES			(5)

#define	MAX_MESSAGES_SIZE		(55)

#define MESSAGE_QUEUE_PATH		(15)

#define PARAMETER_DELIMETER		(" ")

#define FRAME_SOF				(0xAA)

#define APP_CLOSE				(0xFE)

#define FRAME_SOF_OFFSET		(0x00)

#define FRAME_SENSOR_OFFSET		(0x01)

#define FRAME_AXIS_OFFSET		(0x02)

#define FRAME_CS_OFFSET			(0x03)

#define COMMAND_WAIT_DELAY		(8)

#define SHELL_PROMPT			("\n\rUAG>")

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void ShellCommand_WrongSof(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_Accelerometer(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_Magnetometer(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_Gyroscope(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_AllSensors(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_WrongSensor(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_WrongCs(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_WrongAxis(char * CommandParameters[], uint16_t CommandParemeterCount);

static void ShellCommand_Help(char * CommandParameters[], uint16_t CommandParemeterCount);

static void * SensorThread(void* parameter);

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

static shellcmd_t ShellCommands[MAX_COMMANDS_ALLOWED] =
{		
		{"accel", ShellCommand_Accelerometer},
		{"mag", ShellCommand_Magnetometer},
		{"gyro", ShellCommand_Gyroscope},
		{"all", ShellCommand_AllSensors},
		{"wrongsensor", ShellCommand_WrongSensor},
		{"wrongcs", ShellCommand_WrongCs},
		{"wrongaxis", ShellCommand_WrongAxis},
		{"wrongsof", ShellCommand_WrongSof},
		{"help", ShellCommand_Help}
};

const uint8_t MessageQueuePath[MESSAGE_QUEUE_PATH] =
{
		"/messageq"
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////




/* EOF */








typedef struct
{
	int32_t SocketHandler;
	int32_t ThreadId;
	mqd_t * MessageQueue;
}sensorclient_t;

typedef struct
{
	uint8_t  MessagePacket[4];
	uint16_t MessageSize;
}clientmessage_t;




void SendToAllClients(sensorclient_t * ClientList, uint32_t ClientSize, clientmessage_t *Message);





int main(int argc, char * argv[])
{
	int8_t * Command;
	uint8_t CommandIndex;
	int32_t Status;
	int8_t WaitCommand;
	int8_t AmountOfThreads;
	int8_t ClientOffset = 0;
	pthread_t * ClientIds;
	struct mq_attr QueueAttributes;
	mqd_t * ClientQueue;
	sensorclient_t * ClientHandler;
	struct sockaddr_in SocketSettings;
	int32_t ClientSocket;
	uint32_t ServerPort;
	uint8_t * MessageQueueThreadPath;
	uint8_t * ThreadIdAscii;
	uint8_t *Argument;
	uint8_t *CommandToExecute;
	clientmessage_t * MessageToSend;

	if(argc == 4)
	{

		printf("Press enter to continue...");
		fgets((char*)&WaitCommand,2,stdin);

		AmountOfThreads = atoi(argv[1]);

		printf("\n\rCreating %d Threads\n\r",AmountOfThreads);

		/* Create client threads and connect */
		ClientIds = (pthread_t*)malloc(AmountOfThreads*sizeof(pthread_t));

		ClientHandler = (sensorclient_t*)malloc(AmountOfThreads*sizeof(sensorclient_t));

		ServerPort = atoi(argv[3]);

		/* set server settings */
		SocketSettings.sin_family = AF_INET;
		SocketSettings.sin_port = htons(ServerPort);
		SocketSettings.sin_addr.s_addr = inet_addr(argv[2]);

		MessageQueueThreadPath = (uint8_t*)malloc(MESSAGE_QUEUE_PATH);

		ThreadIdAscii = (uint8_t*)malloc(5);

		while(ClientOffset < AmountOfThreads)
		{

			ClientSocket = socket(AF_INET, SOCK_STREAM, 0);


			/*Try to connect the socket for the communication*/
			Status = connect(ClientSocket, (struct sockaddr *)&SocketSettings,sizeof(struct sockaddr));

			if(Status >= 0)
			{
				QueueAttributes.mq_maxmsg = MAX_MESSAGES;
				QueueAttributes.mq_msgsize = MAX_MESSAGES_SIZE;
				QueueAttributes.mq_flags = 0;

				memcpy(MessageQueueThreadPath,&MessageQueuePath[0],MESSAGE_QUEUE_PATH);

				sprintf((char*)ThreadIdAscii, "%d", ClientOffset);

				MessageQueueThreadPath = (uint8_t*)strcat((char*)MessageQueueThreadPath,(char*)ThreadIdAscii);

				ClientQueue = (mqd_t *)malloc(sizeof(mqd_t));

				*ClientQueue  = mq_open((char*)MessageQueueThreadPath, O_CREAT|O_RDWR, MQ_MODE,&QueueAttributes);

				if(ClientQueue != NULL)
				{

					ClientHandler[ClientOffset].MessageQueue = ClientQueue;

					ClientHandler[ClientOffset].SocketHandler = ClientSocket;

					ClientHandler[ClientOffset].ThreadId = ClientOffset;

					/* Connect the client, create the thread with its mq */
					Status = pthread_create(&ClientIds[ClientOffset],NULL,SensorsClient,&ClientHandler[ClientOffset]);
					ClientOffset++;
					/* sleep for 100ms */
					usleep(100000);
				}
				else
				{
					printf("\n\rError creating MQ %d\n\r",ClientOffset);
					free(MessageQueueThreadPath);
					free(ThreadIdAscii);
					free(ClientHandler);
					return (-1);
				}
			}
			else
			{
				printf("\n\rError, no server found\n\r");
				free(MessageQueueThreadPath);
				free(ThreadIdAscii);
				free(ClientHandler);
				return (-1);
			}
		}

		free(MessageQueueThreadPath);
		free(ThreadIdAscii);
        
        while(1)
        {
            
        }
	}
	return (0);

}




void ShellCommand_Accelerometer(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	uint32_t AxisToRequest;

	printf("\n\rAccelerometer Command\n\r");

	if(CommandParameter != NULL)
	{
		AxisToRequest = atoi((char*)CommandParameter);

		if(AxisToRequest && (AxisToRequest <= AXIS_ALL))
		{
			Command = (void)malloc(sizeof(clientmessage_t));
			Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
			Command->MessagePacket[FRAME_SENSOR_OFFSET] = SENSOR_ACCELEROMETER;
			Command->MessagePacket[FRAME_AXIS_OFFSET] = AxisToRequest;
			Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

			Command->MessageSize = 4;

			printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
					Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);

		}
		else
		{
			printf("\n\rWrong command parameter\n\rUsage: accelerometer <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
		}
	}
	else
	{
		printf("\n\rWrong command parameter\n\rUsage: accelerometer <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
	}

	return (Command);
}

void ShellCommand_Magnetometer(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	uint32_t AxisToRequest;

	printf("\n\rMagnetometer Command\n\r");

	if(CommandParameter != NULL)
	{
		AxisToRequest = atoi((char*)CommandParameter);

		if(AxisToRequest && (AxisToRequest <= AXIS_ALL))
		{
			Command = (void)malloc(sizeof(clientmessage_t));
			Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
			Command->MessagePacket[FRAME_SENSOR_OFFSET] = SENSOR_MAGNETOMETER;
			Command->MessagePacket[FRAME_AXIS_OFFSET] = AxisToRequest;
			Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

			Command->MessageSize = 4;

			printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
					Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);

		}
		else
		{
			printf("\n\rWrong command parameter\n\rUsage: magnetometer <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
		}
	}
	else
	{
		printf("\n\rWrong command parameter\n\rUsage: magnetometer <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
	}


	return (Command);
}

void ShellCommand_Gyroscope(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	uint32_t AxisToRequest;

	printf("\n\rGyroscope Command\n\r");

	if(CommandParameter != NULL)
	{
		AxisToRequest = atoi((char*)CommandParameter);

		if(AxisToRequest && (AxisToRequest <= AXIS_ALL))
		{
			Command = (void)malloc(sizeof(clientmessage_t));
			Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
			Command->MessagePacket[FRAME_SENSOR_OFFSET] = SENSOR_GYROSCOPE;
			Command->MessagePacket[FRAME_AXIS_OFFSET] = AxisToRequest;
			Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

			Command->MessageSize = 4;

			printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
					Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);

		}
		else
		{
			printf("\n\rWrong command parameter\n\rUsage: gyroscope <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
		}
	}
	else
	{
		printf("\n\rWrong command parameter\n\rUsage: gyroscope <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
	}



	return (Command);
}

void ShellCommand_AllSensors(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	uint32_t AxisToRequest;

	printf("\n\rAll sensors Command\n\r");

	if(CommandParameter != NULL)
	{
		AxisToRequest = atoi((char*)CommandParameter);

		if(AxisToRequest && (AxisToRequest <= AXIS_ALL))
		{
			Command = (void)malloc(sizeof(clientmessage_t));
			Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
			Command->MessagePacket[FRAME_SENSOR_OFFSET] = SENSOR_ALL;
			Command->MessagePacket[FRAME_AXIS_OFFSET] = AxisToRequest;
			Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

			Command->MessageSize = 4;

			printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
					Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);

		}
		else
		{
			printf("\n\rWrong command parameter\n\rUsage: allsensors <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
		}
	}
	else
	{
		printf("\n\rWrong command parameter\n\rUsage: allsensors <axis>\n\rx = 1\n\ry = 2\n\rz = 3\n\rxyz = 4\n\r");
	}



	return (Command);



}


void ShellCommand_WrongSensor(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	int32_t RandomSensor;
	int32_t RandomAxis;

	printf("\n\rWrong Sensor Command\n\r");

	Command = (void)malloc(sizeof(clientmessage_t));

	srand(time(NULL));

	do
	{
		RandomSensor = rand();
		RandomSensor &= 0xFF;
	}while((RandomSensor <= SENSOR_GYROSCOPE) || (RandomSensor == SENSOR_ALL));

	do
	{
		RandomAxis = rand();
		RandomAxis &= 0xFF;
	}while((RandomAxis > AXIS_ALL) || (RandomAxis == 0));

	Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
	Command->MessagePacket[FRAME_SENSOR_OFFSET] = (uint8_t)RandomSensor;
	Command->MessagePacket[FRAME_AXIS_OFFSET] = (uint8_t)RandomAxis;
	Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

	Command->MessageSize = 4;

	printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
			Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);



	return (Command);
}

void ShellCommand_WrongCs(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	int32_t RandomChecksum;
	int32_t RandomSensor;
	int32_t RandomAxis;
	uint8_t RealChecksum;

	printf("\n\rWrong CS Command\n\r");

	Command = (void)malloc(sizeof(clientmessage_t));

	srand(time(NULL));

		do
	{
		RandomSensor = rand();
		RandomSensor &= 0xFF;
	}while((RandomSensor > SENSOR_GYROSCOPE) || (RandomSensor == 0));

	do
	{
		RandomAxis = rand();
		RandomAxis &= 0xFF;
	}while((RandomAxis > AXIS_ALL) || (RandomAxis == 0));

	Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
	Command->MessagePacket[FRAME_SENSOR_OFFSET] = (uint8_t)RandomSensor;
	Command->MessagePacket[FRAME_AXIS_OFFSET] = (uint8_t)RandomAxis;

	RealChecksum = CalculateChecksum(&Command->MessagePacket[0],3);
	
	do
	{
		RandomChecksum = rand();
		RandomChecksum &= 0xFF;
	}
	while((RandomChecksum ==  0xFF) || (RandomChecksum == 0) || (RandomChecksum == RealChecksum));


	printf("\n\rReal Checksum = 0x%X\n\rRandom Checksum = 0x%X\n\r",RealChecksum,RandomChecksum);

	Command->MessagePacket[FRAME_CS_OFFSET] = (uint8_t)(RealChecksum & RandomChecksum);

	Command->MessageSize = 4;

	printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
			Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);


	return (Command);
}

void ShellCommand_WrongAxis(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	int32_t RandomSensor;
	int32_t RandomAxis;

	printf("\n\rWrong Axis Command\n\r");

	Command = (void)malloc(sizeof(clientmessage_t));

	srand(time(NULL));

	do
	{
		RandomSensor = rand();
		RandomSensor &= 0xFF;
	}while((RandomSensor > SENSOR_GYROSCOPE) || (RandomSensor == 0));

	do
	{
		RandomAxis = rand();
		RandomAxis &= 0xFF;
	}while((RandomAxis <= AXIS_ALL));

	Command->MessagePacket[FRAME_SOF_OFFSET] = FRAME_SOF;
	Command->MessagePacket[FRAME_SENSOR_OFFSET] = (uint8_t)RandomSensor;
	Command->MessagePacket[FRAME_AXIS_OFFSET] = (uint8_t)RandomAxis;
	Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

	Command->MessageSize = 4;

	printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
			Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);

	return (Command);
}

clientmessage_t *ShellCommand_WrongSof(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	int32_t RandomSensor;
	int32_t RandomAxis;
	int32_t RandomSof;

	printf("\n\rWrong SOF Command\n\r");

	Command = (void)malloc(sizeof(clientmessage_t));

	srand(time(NULL));

	do
	{
		RandomSensor = rand();
		RandomSensor &= 0xFF;
	}while((RandomSensor > SENSOR_GYROSCOPE) || (RandomSensor == 0));

	do
	{
		RandomAxis = rand();
		RandomAxis &= 0xFF;
	}while((RandomAxis >= AXIS_ALL) || (RandomAxis == 0));

	do
	{
		RandomSof = rand();
		RandomSof &= 0xFF;

	}while(RandomSof == FRAME_SOF);

	Command->MessagePacket[FRAME_SOF_OFFSET] = RandomSof;
	Command->MessagePacket[FRAME_SENSOR_OFFSET] = (uint8_t)RandomSensor;
	Command->MessagePacket[FRAME_AXIS_OFFSET] = (uint8_t)RandomAxis;
	Command->MessagePacket[FRAME_CS_OFFSET] = CalculateChecksum(&Command->MessagePacket[0],3);

	Command->MessageSize = 4;

	printf("\n\rFrame to send: 0x%X 0x%X 0x%X 0x%X\n\r",Command->MessagePacket[FRAME_SOF_OFFSET], Command->MessagePacket[FRAME_SENSOR_OFFSET],\
			Command->MessagePacket[FRAME_AXIS_OFFSET],Command->MessagePacket[FRAME_CS_OFFSET]);

	return (Command);
}

void ShellCommand_Help(char * CommandParameters[], uint16_t CommandParemeterCount)
{
	clientmessage_t * Command = NULL;
	uint32_t TotalCommands = sizeof(ShellCommands)/sizeof(ShellCommands[0]);
	uint16_t CommandOffset = 0;

	printf("Available commands:\n\r\n\r");
	(void)Command;
	while(CommandOffset < TotalCommands)
	{
		printf("%s\n\r",(char*)ShellCommands[CommandOffset].CommandString);
		CommandOffset++;
	}

	return(Command);
}

void * SensorThread(void* parameter)
{
	sensorclient_t * SensorCommands = (sensorclient_t*)(parameter);
	ssize_t DataReceived;
	ssize_t ServerStatus;
	uint8_t *MessageBuffer = (uint8_t*)malloc(MAX_MESSAGES_SIZE);
	uint8_t *ServerBuffer = (uint8_t*)malloc(MAX_MESSAGES_SIZE);
	uint32_t PrintFrame = 0;
	uint8_t ReceivedChecksum;
	uint16_t ChecksumSize;

	printf("\n\rClient %d created\n\r",SensorCommands->ThreadId);

	while(1)
	{
		/* Waits for a message. The Thread will block until there's a message */
		DataReceived = mq_receive(*SensorCommands->MessageQueue,(char*)MessageBuffer,MAX_MESSAGES_SIZE,NULL);

		if(DataReceived)
		{
			if(MessageBuffer[0] == FRAME_SOF)
			{

				/* send data to the server*/
				ServerStatus = write(SensorCommands->SocketHandler,MessageBuffer,DataReceived);

				/* wait for server response */
				ServerStatus = read(SensorCommands->SocketHandler,ServerBuffer,MAX_MESSAGES_SIZE);

				printf("\n\rFrame received at thread %d of size %d: ",SensorCommands->ThreadId,(uint32_t)ServerStatus);
				while(ServerStatus--)
				{
					printf("0x%X ",ServerBuffer[PrintFrame]);
					PrintFrame++;
				}
				printf("\n\r");
				PrintFrame = 0;
				
				if(ServerBuffer[1] != 0xFE)
				{

					ChecksumSize = ServerBuffer[2] + 1;
					ReceivedChecksum = CalculateChecksum(&ServerBuffer[0],ChecksumSize);

					printf("\n\rCalculated checksum = 0x%X\n\r",ReceivedChecksum);
				}
				memset(ServerBuffer,0,MAX_MESSAGES_SIZE);
			}
			else if(MessageBuffer[0] == APP_CLOSE)
			{
				printf("\n\rCleaning and closing thread %d:\n\r",SensorCommands->ThreadId);
				close(SensorCommands->SocketHandler);
				free(MessageBuffer);
				free(ServerBuffer);
				mq_close(*SensorCommands->MessageQueue);
				usleep(1000);
				break;
			}
			else
			{

				printf("\n\rWrong SOF\n\r");
			}

			memset(MessageBuffer,0,MAX_MESSAGES_SIZE);

		}
	}


	pthread_exit(NULL);
}

void SendToAllClients(sensorclient_t * ClientList, uint32_t ClientSize, clientmessage_t *Message)
{
	uint32_t Clients = 0;

	while (Clients < ClientSize)
	{
		mq_send(*ClientList[Clients].MessageQueue,(char*)Message->MessagePacket,Message->MessageSize,0);

		Clients++;

	}
}



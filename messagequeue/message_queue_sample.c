///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h> 
#include <stdint.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MQ_MODE       (S_IRWXU | S_IRWXG | S_IRWXO)

#define INCREMENT_COMMAND	('i')

#define MAX_MESSAGES		(10)

#define	MAX_MESSAGES_SIZE	(10)

#define COMMAND_BUFFER_SIZE	(MAX_MESSAGES * MAX_MESSAGES_SIZE)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void *ThreadPrint(void *ThreadParameter);

void *ThreadIncrement(void *ThreadParameter);

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

int main (int argc, char *argv[])
{
	int32_t QueueFlags = 0;
	struct mq_attr QueueAttributes;
	mqd_t mqd;
	pthread_t ThreadPrintId;
	pthread_t ThreadIncrementId;

    /* Create the new message queue */
	QueueFlags =  O_CREAT|O_RDWR;
	QueueAttributes.mq_maxmsg = MAX_MESSAGES;
	QueueAttributes.mq_msgsize = MAX_MESSAGES_SIZE;
	QueueAttributes.mq_flags = 0;
 
	mqd = mq_open("/Message_queue_test", QueueFlags, MQ_MODE,&QueueAttributes);


	if(mqd < 0)
	{

		perror("MessageCreate");

	}
	/* Threads will share the queue */
	(void)pthread_create(&ThreadPrintId, NULL, &ThreadPrint, (void*)&mqd);
	(void)pthread_create(&ThreadIncrementId, NULL, &ThreadIncrement, (void*)&mqd);


	while(1)
	{


	}	
}


void *ThreadPrint(void *ThreadParameter)
{
	mqd_t * PrintMessageQueue = (mqd_t*)ThreadParameter;
	char * CommandBuffer = (char*)malloc(COMMAND_BUFFER_SIZE);

    if(CommandBuffer)
    {
        printf("Print Message Queue ID: %d\n", *PrintMessageQueue);

        while(1)
        {
            /* Wait for user input and send it*/
            printf("Write a command:\n");
            
            fgets(CommandBuffer,COMMAND_BUFFER_SIZE,stdin);
            
            /* Just the first character will be sent, hence the '1'*/
            (void)mq_send(*PrintMessageQueue,CommandBuffer,1,0);
            
        }
    }
    else
    {
        printf("Error allocating buffer");
    }

    pthread_exit(NULL);
}

void *ThreadIncrement(void *ThreadParameter)
{

	mqd_t * IncrementMessageQueue = (mqd_t*)ThreadParameter;
	uint32_t MessageCounter = 0;
	int32_t RxStatus;
    char * RxBuffer = (char *)malloc(COMMAND_BUFFER_SIZE);
    
    if(RxBuffer)
    {
	
        printf("Increment Message Queue ID: %d\n", *IncrementMessageQueue);

        while(1)
        {
            
            /* Waits for a message. The Thread will block until there's a
             * message */
            RxStatus = mq_receive(*IncrementMessageQueue,RxBuffer,COMMAND_BUFFER_SIZE,NULL);
          
            if(RxStatus > 0)
            {
                if(*RxBuffer  == INCREMENT_COMMAND)
                {
                    MessageCounter++;
                    printf("Incrementing Counter: %d\n",MessageCounter);
                }
                else
                {
                    printf("Wrong Command\n");
                }
            }
            else
            {
                perror("ReadMSG");
            }
        }
    }
    else
    {
         printf("Error allocating buffer");
    }
    
    pthread_exit(NULL);
}


/* EOF */




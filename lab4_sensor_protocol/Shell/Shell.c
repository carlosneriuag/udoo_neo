///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include <unistd.h>
#include "Shell.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    shellcmd_t * ShellTable;
    uint16_t TableSize;
    char * Prompt;
}shellhandle_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void * Shell_Thread(void* Handle);


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static char * const WelcomeMessage[] =
{
    "\n\n\n\r\
     ## # # ### #   #\n\r\
    #   # # #   #   #\n\r\
     #  ### ##  #   #\n\r\
      # # # #   #   #\n\r\
    ##  # # ### ### ###\n\r"    
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////


int32_t Shell_Initialization(shellcmd_t * CommandTable, uint16_t NumberOfCommands, char * AppPrompt)
{
    int32_t ShellStatus = SHELL_ERROR;
    shellhandle_t * ShellHandle = NULL;
    int32_t Status;
    pthread_t ThreadId;
    
    if((CommandTable != NULL) && (NumberOfCommands > 0))
    {
        ShellHandle = (shellhandle_t*)malloc(sizeof(shellhandle_t));
        
        if(ShellHandle != NULL)
        {        
            ShellHandle->ShellTable = CommandTable;
            ShellHandle->TableSize = NumberOfCommands;
            ShellHandle->Prompt = AppPrompt;
            
            #ifdef DEBUG
            printf("DEBUG %s: TableSize = %d\n\r",__FUNCTION__,ShellHandle->TableSize);
            #endif
            
            Status = pthread_create(&ThreadId,NULL,Shell_Thread,(void*)ShellHandle);
            
            if(Status > 0)
            {   
                ShellStatus = SHELL_OK;
            }
        }
    }
    
    return ShellStatus;
}

static void * Shell_Thread(void* Handle)
{
    shellhandle_t * ShellHandle;
    int32_t Status;
    char * Command;
	char * CommandToExecute;
    char * * ParemetersList;
    char * NewParameter;
    uint16_t CommandIndex;
    uint16_t ParameterCounter;
    
    ShellHandle = (shellhandle_t *)Handle;
    
    ParemetersList = (char **)malloc(SHELL_ARGUMENT_MAX * sizeof(char *));
    
    if(ParemetersList != NULL)
    {
        
        printf("%s",WelcomeMessage[0]);
        printf("     %s\n\r",__DATE__);
        
        while(1)
        {
            printf("\n\r");
            Command = readline(ShellHandle->Prompt);
            
            if(Command)
            {
                /* add the command to the history and process it after */
                add_history(Command);
                
                CommandIndex = 0;
                ParameterCounter = 0;
                /* extract the command */
                CommandToExecute = strtok(Command," ");
                
                if(CommandToExecute != NULL)
                {
                    /* get the parameters */
                    while(ParameterCounter < SHELL_ARGUMENT_MAX)
                    {
                        NewParameter = strtok(NULL," ");
                        
                        if(NewParameter != NULL)
                        {
                            ParemetersList[ParameterCounter] = NewParameter;
                            ParameterCounter++;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    /* look if the command exits */
                    while(CommandIndex < ShellHandle->TableSize)
                    {
                        Status = strcmp(CommandToExecute,ShellHandle->ShellTable[CommandIndex].CommandString);

                        /* once the command is found, execute the callback */
                        if(!Status)
                        {
                            ShellHandle->ShellTable[CommandIndex].CommandCallback(ParemetersList, ParameterCounter);

                            break;
                        }
                        
                        CommandIndex++;
                    }
                    
                          
                    if(CommandIndex >= ShellHandle->TableSize)
                    {
                        
                        Status = strcmp(CommandToExecute,"help");
                    
                        /* print commands */
                        if(!Status)
                        {
                            CommandIndex = 0;
                            
                            while(CommandIndex < ShellHandle->TableSize)
                            {
                                printf("%s\n\r",ShellHandle->ShellTable[CommandIndex].CommandString);
                                CommandIndex++;
                            }
                        }
                        else
                        {
                            printf("\n\rCommand not found\n\r");
                        }
                    }
                }
            }
        }
    }
    else
    {
        printf("Error while creating Shell thread");
    }
    
    pthread_exit(NULL);
}

/* EOF */
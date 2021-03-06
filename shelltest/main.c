///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "Shell.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void ShellCommand_Add(char * CommandParameters[], uint16_t CommandParemeterCount);

void ShellCommand_Subtract(char * CommandParameters[], uint16_t CommandParemeterCount);

void ShellCommand_Multiply(char * CommandParameters[], uint16_t CommandParemeterCount);

void ShellCommand_Divide(char * CommandParameters[], uint16_t CommandParemeterCount);

void ShellCommand_Exit(char * CommandParameters[], uint16_t CommandParemeterCount);
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static  char * const WelcomeMessage[] =
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

shellcmd_t ShellCommands[] =
{		
    {"add", ShellCommand_Add},
    {"sub", ShellCommand_Subtract},
    {"mul", ShellCommand_Multiply},
    {"div", ShellCommand_Divide},
    {"exit", ShellCommand_Exit}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////


int32_t main(int32_t argc, char * argv[])
{
    
    Shell_Initialization(&ShellCommands[0],sizeof(ShellCommands)/sizeof(ShellCommands[0]),"shell>");
    
    printf("%s",WelcomeMessage[0]);
    printf("     %s\n\r",__DATE__);
    
    while(1)
    {
     
    }
}

void ShellCommand_Add(char * CommandParameters[], uint16_t CommandParemeterCount)
{
    uint32_t FirstAddend;
    uint32_t SecondAddend;
    uint32_t Result;
    
    if(CommandParemeterCount == 2)
    {
        FirstAddend = atoi(CommandParameters[0]);
        
        SecondAddend = atoi(CommandParameters[1]);
        
        Result = FirstAddend + SecondAddend;
        
        printf("Result = %d\n\r", Result);
    }
    else
    {
        printf("Wrong parameter number.\n\rUsage add x y\n\r");
    }
}

void ShellCommand_Subtract(char * CommandParameters[], uint16_t CommandParemeterCount)
{
    uint32_t FirstOperand;
    uint32_t SecondOperand;
    uint32_t Result;
    
    if(CommandParemeterCount == 2)
    {
        FirstOperand = atoi(CommandParameters[0]);
        
        SecondOperand = atoi(CommandParameters[1]);
        
        Result = FirstOperand - SecondOperand;
        
        printf("Result = %d\n\r", Result);
    }
    else
    {
        printf("Wrong parameter number.\n\rUsage add x y\n\r");
    }  
}

void ShellCommand_Multiply(char * CommandParameters[], uint16_t CommandParemeterCount)
{
    uint32_t FirstOperand;
    uint32_t SecondOperand;
    uint32_t Result;
    
    if(CommandParemeterCount == 2)
    {
        FirstOperand = atoi(CommandParameters[0]);
        
        SecondOperand = atoi(CommandParameters[1]);
        
        Result = FirstOperand * SecondOperand;
        
        printf("Result = %d\n\r", Result);
    }
    else
    {
        printf("Wrong parameter number.\n\rUsage add x y\n\r");
    }
}

void ShellCommand_Divide(char * CommandParameters[], uint16_t CommandParemeterCount)
{
    uint32_t FirstOperand;
    uint32_t SecondOperand;
    uint32_t Result;
    
    if(CommandParemeterCount == 2)
    {
        FirstOperand = atoi(CommandParameters[0]);
        
        SecondOperand = atoi(CommandParameters[1]);
        
        Result = FirstOperand / SecondOperand;
        
        printf("Division is integer only \n\r Result = %d\n\r", Result);
    }
    else
    {
        printf("Wrong parameter number.\n\rUsage add x y\n\r");
    }
}

void ShellCommand_Exit(char * CommandParameters[], uint16_t CommandParemeterCount)
{
    printf("\n\rGoodbye\n\r");
    
    exit(1);
}
/* EOF */
#include <stdint.h>
#include <stdio.h>
#include <signal.h>

void ExitApplication(int32_t Signal);

volatile uint8_t ExitAppFlag = 0;

volatile int32_t SignalReceived;

int main(void)
{
    /* https://linux.die.net/man/2/signal */
    
    
    /* Register the callback */
    (void)signal(SIGINT,ExitApplication);


    printf("Application started and signal callback registered\n\r");
    printf("Press CTRL+C to finish\n\r");


    while(1)
    {
        if(ExitAppFlag == 1)
        {
            if(SignalReceived == SIGINT)
            {
                printf("Interrupt from keyboard, ending application\n\r");
            }
            else
            {
                printf("Unexpected signal, ending application\n\r");
            }
                        
            /* usually, a message to threads is sent and all */
            /* resources are free'd                          */
            break;
	
        }

    }
    
    return 0;
}

void ExitApplication(int32_t Signal)
{

    ExitAppFlag = 1;

    SignalReceived = Signal;
}
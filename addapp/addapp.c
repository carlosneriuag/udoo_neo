#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int32_t main (int32_t argc, char * argv[])
{
    int32_t FirstNumber;
    int32_t SecondNumber;
    int32_t SumResult;

    /* Make sure we receive correct number of parameters */

    if(argc == 3)
    {

        /* Parameters are ascii, hence, convert to integer for this use case  */
        FirstNumber = atoi(argv[1]);

    	SecondNumber = atoi(argv[2]);

	    SumResult = FirstNumber + SecondNumber;


	    printf("\n\r%d + %d = %d\n\r\n\r",FirstNumber, SecondNumber,SumResult);

    }
    else
    {
        
        printf("\n\rWrong number of parameters\n\rUsage: ./addapp X Y\n\r\n\r");
    
    }
    
    return 0;
}
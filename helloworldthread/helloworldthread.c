#include <stdio.h>
#include <stdint.h>
#include <pthread.h>


void ThreadHelloWorld(void * argv);


int main (void)
{
    pthread_t threadHello;
    
    printf("Creating a thread\n");  
                
    /* Create the thread */
    (void)pthread_create(&threadHello, NULL, (void*)ThreadHelloWorld, (void*) NULL);
    
    /* Pthread join blocks the current thread until the specified one is done */
    (void)pthread_join(threadHello,NULL);

    printf("Exiting now!!\n");
    return(1);
}

void ThreadHelloWorld(void * argv)
{

    printf("Hellow World from Thread\n");
            
    pthread_exit(NULL);
}

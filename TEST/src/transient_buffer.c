#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "transient_buffer.h"

#define BUFFERSIZE  65535
int in, out;
unsigned char data_buffer[BUFFERSIZE];
pthread_mutex_t lock; 
pthread_cond_t cond;

void transient_buffer_init(void)
{
    in = 0;
    out = 0;
    transient_buffer_reset();
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
}

void transient_buffer_destroy(void)
{
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

void write_transient_buffer(unsigned char data)
{
    /*enter critical section*/
    pthread_mutex_lock(&lock);
    /*wait for space in buffer*/
    while(((in + 1) % BUFFERSIZE) == out)
    {
        printf("transient buffer full\n");
        if(pthread_cond_wait(&cond, &lock))
        {
            printf("pthread_cond_wait:producer\n");
            exit(-1);
        }
    }
//    printf("in:%d\n",in);
    /*put value into the buffer*/    
    data_buffer[in] = data;
    in = (in + 1) % BUFFERSIZE;
    /*leave critical section*/
    pthread_mutex_unlock(&lock);
    /*wake up at least one consumer*/
    pthread_cond_signal(&cond);
}

unsigned char read_transient_buffer(void)
{
    /*enter critical section*/
    pthread_mutex_lock(&lock);
    /*wait for data in the buffer*/
    while(out == in)
        if(pthread_cond_wait(&cond, &lock))
        {
            printf("pthread_cond_wait:consumer\n");
            exit(-1);
        }
//    printf("out:%d\n", out);
    unsigned char result = data_buffer[out];
    
    /*wake up the producer if the buffer was full; the producer
      will stay blocked until we exit the critical section, below*/
    if(((in + 1) % BUFFERSIZE) == out)
        pthread_cond_signal(&cond);
    out = (out + 1) % BUFFERSIZE;    
    /*leave critical section*/
    pthread_mutex_unlock(&lock);
    return result;
}
void transient_buffer_reset(void)
{
    int i;
    
    for(i=0;i<BUFFERSIZE;i++)
    {
        data_buffer[i] = 0x00;
    }
}


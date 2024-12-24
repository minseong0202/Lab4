#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 5  
#define NUM_PRODUCERS 2 
#define NUM_CONSUMERS 2  

int buffer[MAX];  
int count = 0;    
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; 
pthread_cond_t fill = PTHREAD_COND_INITIALIZER; 


void put(int value, int producer_id) {
    buffer[count] = value;
    count++;
    printf("Producer %d: %d를 생산했다., Buffer count: %d\n", producer_id, value, count);
}


int get(int consumer_id) {
    int value = buffer[count - 1];
    count--;
    printf("Consumer %d: %d를 소비했다., Buffer count: %d\n", consumer_id, value, count);
    return value;
}


void *producer(void *arg) {
    int id = *((int *)arg); 
    while (1) {
        pthread_mutex_lock(&m); 
        while (count == MAX) {  
            pthread_cond_wait(&empty, &m);  
        }
        int value = rand() % 100;  
        put(value, id);  
        pthread_cond_signal(&fill);  
        pthread_mutex_unlock(&m);  
        sleep(1);  
    }
}


void *consumer(void *arg) {
    int id = *((int *)arg);  
    while (1) {
        pthread_mutex_lock(&m);  
        while (count == 0) {  
            pthread_cond_wait(&fill, &m);  
        }
        get(id);  
        pthread_cond_signal(&empty);  
        pthread_mutex_unlock(&m);  
        sleep(1);  
    }
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS], consumer_ids[NUM_CONSUMERS];

    
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        consumer_ids[i] = i + 1;  
        if (pthread_create(&producers[i], NULL, producer, &producer_ids[i]) != 0) {
            perror("Failed to create producer thread");
        }
        if (pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]) != 0) {
            perror("Failed to create consumer thread");
        }
    }


    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }


    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}

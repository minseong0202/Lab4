#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h> 

#define BUFFER_SIZE 20
#define NUMITEMS 30

// 생산자-소비자 문제 해결 예제
// 두개의 쓰레드(하나는 생산자, 하나는 소비자)가 공유자원(여기선 버퍼)을 안전하게 사용하도록 동기화

typedef struct {
    int item[BUFFER_SIZE]; // 버퍼사이즈만큼 크기를 갖는 배열. 항목들 저장
    int totalitems; // 버퍼에 현재 저장된 항목 수 추적
    int in, out; // 버퍼에서 항목을 삽입하는 인덱스(in), 꺼내는 인덱스(out)
    pthread_mutex_t mutex; // 버퍼에 대한 동기화를 위해 사용되는 뮤텍스
    pthread_cond_t full; // 조건변수 (full은 버퍼가 가득 찾을 때)
    pthread_cond_t empty; // 조건변수 (empty는 버퍼가 비었을 때 대기기)
}
buffer_t;

buffer_t bb = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0,
                PTHREAD_MUTEX_INITIALIZER,
                PTHREAD_COND_INITIALIZER,
                PTHREAD_COND_INITIALIZER
                }; // buffer_t 타입의 전역 변수 : 공유 버퍼

int produce_item() { // 새로운 항목 생성
    int item = (int)(100.0 * rand() / (RAND_MAX + 1.0));
    sleep((unsigned long)(5.0 * rand() / (RAND_MAX + 1.0)));
    printf("produce_item: item=%d\n", item);
    return item;
}

// 생성자가 호출하여 생성된 하목을 버퍼에 삽입
int insert_item(int item) {
    
    // pthread_mutex_lock()을 호출하여 버퍼에 대한 뮤텍스를 잠근다.
    int status = pthread_mutex_lock(&bb.mutex);
    if (status != 0)
        return status;

    // 버퍼가 가득차면 생성자는 pthread_cond_wait()을 호출하여 조건 변수 empty에서 대기하고,
    // 소비자가 항목을 제거하면 대기 상태에서 벗어난다.
    // 대기 중에는 다른 쓰레드가 버퍼를 사용하여 상태가 변할 수 있다.
    while (bb.totalitems >= BUFFER_SIZE) {
        status = pthread_cond_wait(&bb.empty, &bb.mutex);  
        if (status != 0) {
            pthread_mutex_unlock(&bb.mutex);
            return status;
        }
    }

    bb.item[bb.in] = item;
    bb.in = (bb.in + 1) % BUFFER_SIZE;
    bb.totalitems++;

    // 소비자 쓰레드에게 버퍼에 항목이 추가되었다고 알린다.
    // 소비자는 버퍼가 비어 있으면 full 조건 변수에서 대기하고, 생산자가 항목을 삽입하면
    // 대기 상태에서 벗어난다.
    status = pthread_cond_signal(&bb.full);  
    if (status != 0) {
        pthread_mutex_unlock(&bb.mutex);
        return status;
    }

    // 작업을 마치고 pthread_mutex_unlock를 호출하여 뮤텍스를 해제
    pthread_mutex_unlock(&bb.mutex);
    return 0;
}

// 소비자가 호출하여 항목을 소비하는 함수
void consume_item(int item) {
    sleep((unsigned long)(5.0 * rand() / (RAND_MAX + 1.0)));
    printf("\t\tconsume_item: item=%d\n", item);
}


// 소비자가 호출하여 버퍼에서 항목을 제거
int remove_item(int *temp) {
    int status = pthread_mutex_lock(&bb.mutex);
    if (status != 0)
        return status;

    while (bb.totalitems <= 0) {
        status = pthread_cond_wait(&bb.full, &bb.mutex); 
        if (status != 0) {
            pthread_mutex_unlock(&bb.mutex);
            return status;
        }
    }

    *temp = bb.item[bb.out];
    bb.out = (bb.out + 1) % BUFFER_SIZE;
    bb.totalitems--;

    status = pthread_cond_signal(&bb.empty);  
    if (status != 0) {
        pthread_mutex_unlock(&bb.mutex);
        return status;
    }

    pthread_mutex_unlock(&bb.mutex);
    return 0;
}

// 생성자 함수
// 생성자 쓰레드가 반복적으로 produce_item을 호출하여 항목을 생성하고, insert_item을 호출하여 버퍼에 삽입
// 항목을 생성하고, 버퍼에 삽입
void *producer(void *arg) {
    int item;
    while (1) {
        item = produce_item();
        insert_item(item);
    }
}

// 소비자 함수
// 버퍼에서 항목을 제거하고, 소비 
void *consumer(void *arg) {
    int item;
    while (1) {
        remove_item(&item);
        consume_item(item);
    }
}

int main() {
    int status;
    void *result;
    pthread_t producer_tid, consumer_tid;


    // 생산자 쓰레드와 소비자 쓰레드 생성
    status = pthread_create(&producer_tid, NULL, producer, NULL);
    if (status != 0)
        perror("Create producer thread");

    status = pthread_create(&consumer_tid, NULL, consumer, NULL);
    if (status != 0)
        perror("Create consumer thread");


    // 각각의 쓰레드가 종료될 때까지 기다린다.
    status = pthread_join(producer_tid, &result);
    if (status != 0)
        perror("Join producer thread");

    status = pthread_join(consumer_tid, &result);
    if (status != 0)
        perror("Join consumer thread");

    return 0;
}


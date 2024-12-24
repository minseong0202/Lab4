#include <stdio.h>
#include <stdlib.h>  
#include <pthread.h> 
#include <unistd.h>

#define NUM_THREADS 3

pthread_mutex_t mutex; // 뮤텍스 객체 선언
int sum = 0; // 공유 변수 : 모든 쓰레드가 더할 값을 저장

// 쓰레드 함수 : 뮤텍스를 사용하여 안전하게 sum에 값을 더함
void *mutex_thread(void *arg) {
    int value = (int)(intptr_t)arg; // 인자를 정수로 변환
    pthread_mutex_lock(&mutex); // 뮤텍스 잠금
    sum += value; // 공유 자원 접근                
    pthread_mutex_unlock(&mutex); // 뮤텍스 해제
    return NULL; 
}

int main(int argc, char *argv[]) {
    pthread_t tid[NUM_THREADS];  
    int arg[NUM_THREADS];    
    int i, status;

    if (argc < 4) { // 명령줄 인자를 3개 받아야한다.
        fprintf(stderr, "Usage: mutexthread number1 number2 number3\n");
        exit(1);
    }

    
    for (i = 0; i < NUM_THREADS; i++) {
        arg[i] = atoi(argv[i + 1]); // 명령줄에서 입력받은 숫자를 정수형으로 변환
    }

    pthread_mutex_init(&mutex, NULL); // 뮤텍스 객체 초기화

   
   // 쓰레드 생성
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_create(&tid[i], NULL, mutex_thread, (void *)(intptr_t)arg[i]);
        if (status != 0) { 
            fprintf(stderr, "Create thread %d: %d\n", i, status);
            exit(1);
        }
    }

   
   // 쓰레드 종료 대기
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_join(tid[i], NULL);
        if (status != 0) { 
            fprintf(stderr, "Join thread %d: %d\n", i, status);
            exit(1);
        }
    }


    // 뮤텍스 파괴
    status = pthread_mutex_destroy(&mutex);
    if (status != 0) {
        perror("Destroy mutex");
    }

  
    printf("sum is %d\n", sum);
    return 0;
}
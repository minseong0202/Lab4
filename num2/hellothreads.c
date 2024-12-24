#include<stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_THREADS 3 // 쓰레드 개수가 3개

void *hello_thread(void *arg){ // arg로 전달된 값을 쓰레드 ID로 출력
                               // void *arg : 쓰레드 생성될 때 전달하는 값
    printf("Thread %d: 이건 쓰레드야\n", (int)(intptr_t)arg);
    return arg;
}

int main(){
    pthread_t tid[NUM_THREADS]; // 쓰레드 ID를 저장하는 배열
    int i, status;

    for(i=0; i<NUM_THREADS; i++){
        status=pthread_create(&tid[i], NULL,hello_thread,(void*)(intptr_t) i);
        // &tid[i] : 생성될 쓰레드의 ID를 저장할 변수 주소
        // NULL : 쓰레드 속성
        // hello_thread : 쓰레드 시작 루틴
        // (void*) i : 쓰레드 시작 루틴이 i를 넘겨받음음
        if(status!=0){
            fprintf(stderr,"Createthread  %d:%d", i,status);
            exit(1);
        }
    
    }
    pthread_exit(NULL);
}
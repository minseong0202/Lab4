#include <pthread.h>
#include <stdio.h>

void *hello_thread(void*arg){
    printf("Thread: Hello, World!\n");
    return arg;
}

int main(){
    pthread_t tid;
    // tid는 pthread_t 타입의 변수
    // 생성된 쓰레드의 ID를 저장한다.

    int status;

    // 쓰레드 생성 함수
    status=pthread_create(&tid, NULL, hello_thread,NULL); // 쓰레드 생성
    // &tid는 생성될 쓰레드의 ID가 저장될 위치의 주소
    // tid는 pthread_t 타입의 변수이다. tid를 바로 넘기면 변수를 직접 수정할 수 없어서 주소를 전달해야함
    // 쓰레드 속성 : NULL
    // 새 쓰레드가 실행할 함수의 주소 : hello_thread
    // 쓰레드 시작 루틴이 넘겨받는 인자 : NULL

    if(status!=0){
        perror("Create thread");
    }

    // 쓰레드 종료 함수
    pthread_exit(NULL);
    // main함수를 수행하는 초기 쓰레드만 종료하기 위해 pthread_exit(NULL); 호출
}
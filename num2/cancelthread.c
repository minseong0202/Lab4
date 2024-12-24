#include <stdio.h> 
#include <pthread.h>
#include <unistd.h> // sleep 함수 사용용
#include <stdlib.h> // exit() 및 atoi() 함수 사용
#include <stdint.h> // intptr_t를 사용해 포인터를 안전하게 정수형으로 변환


// 쓰레드 취소 요청 확인
void *cancel_thread(void *arg) { // 쓰레드를 생성할 때 쓰레드에 넘겨줄 인자를 받는 역할 : void *arg
    int i, state;

    for (i = 0;; i++) { // ;;은 무한루프이다.

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state);
        // 쓰레드 취소 상태 비활성화(쓰레드 취소 요청 받지 않음음)
        // PTHREAD_CANCEL_DISABLE : 쓰레드가 취소되지 않도록 설정
        // &state : 이전 상태 저장

        printf("Thread: cancel state disabled\n");
        sleep(1); // 1초 동안 현재 쓰레드 멈춤


        pthread_setcancelstate(state, &state);
        // 이전에 저장된 쓰레드 취소 상태 복원
        // 취소 상태가 활성화로 되돌려진다.

        printf("Thread: cancel state restored\n");

        if (i % 5 == 0) {
            pthread_testcancel(); // 쓰레드 취소 요청 점검
                                  // 쓰레드가 취소 상태일때만 작동
                                  // 여기서 쓰레드 취소 상태를 비활성화하고 원래 상태로 복원하고 있다.
                                  // 취소 상태가 항상 활성화되지 않아서 이 함수는 안쓰는거와 마찬가지지
        }
    }

    return arg;
}

int main(int argc, char *argv[]) { // 명령줄 인자이다. 프로그램 실행되면 전달한다.
    // argc는 프로그램 이름을 포함하여 명령줄에서 전달된 문자열의 개수
    // ex) ./cancelthread 5 이거면 2개인거다

    // argv는 명령줄 인자를 문자열 배열로 저장한다.
    // ex) ./cancelthread 5 이거면 argv[1]은 5이다.

    pthread_t tid; // 쓰레드 ID
    int arg, status;
    void *result;

    if (argc < 2) {
        fprintf(stderr, "Usage: cancelthread <time(sec)>\n");
        exit(1);
    }

    status = pthread_create(&tid, NULL, cancel_thread, NULL);
    // &tid : 생성된 쓰레드 ID 저장
    // NULL : 기본 속성
    // cancel_thread : 쓰레드에서 실행할 함수
    // NULL : 함수에 전달할 인자

    if (status != 0) {
        fprintf(stderr, "Create thread failed: %d\n", status);
        exit(1);
    }

    arg = atoi(argv[1]);
    sleep(arg);          


    status = pthread_cancel(tid); // 쓰레드를 취소
                                  // 취소되기를 바라는 쓰레드, 취소 요청이 허용되면 쓰레드는 모든 정리 및 종료 작업 수행
                                  // 생성된 쓰레드를 취소하려고하지만, pthread_setcancelstate 이것 때문에 실제로 취소x
    
    if (status != 0) {
        fprintf(stderr, "Cancel thread failed: %d\n", status);
        exit(1);
    }

    status = pthread_join(tid, &result); // 쓰레드 종료 대기기
    if (status != 0) {
        fprintf(stderr, "Join thread failed: %d\n", status);
        exit(1);
    }

    return (int)(intptr_t)result;


    // 취소를 활성화해야 쓰레드가 취소될 수 있다.
}
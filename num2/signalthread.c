#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define NUM_THREADS 3

// POSIX 쓰레드와 시그널(SIGINT)을 사용하여 쓰레드 동기화 및 시그널 처리
// SIGINT 시그널(Ctrl+C)을 처리하는 쓰레드 생성

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex : 쓰레드 간 동기화를 위한 뮤텍스
                                                   // 읽기-쓰기 잠금 초기화
sigset_t sigset; // 시그널 집합. SIGINT 시그널만 추가
int completed = 0; // 시그널이 3번 발생했음을 확인하기 위한 플래그 변수

void *signal_thread(void *arg) {
    int signal;
    int count = 0;

    while (1) {
        sigwait(&sigset, &signal); // 지정된 시그널 집합 sigset에서 시그널 기다림

        if (signal == SIGINT) { // SIGINT가 감지되면 카운트 증가
            printf("Signal thread: SIGINT %d\n", ++count);

            if (count >= 3) {
                pthread_mutex_lock(&mutex); // 뮤텍스 잠금
                completed = 1; // 완료 플래그 설정 
                pthread_mutex_unlock(&mutex); // 뮤텍스 해제
                break;
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    int arg;
    int status;

    if (argc < 2) {
        fprintf(stderr, "Usage: signalthread time(sec)\n");
        exit(1);
    }

    arg = atoi(argv[1]);

    sigemptyset(&sigset); // 시그널 집합을 비운다.
    sigaddset(&sigset, SIGINT); // SIGINT 시그널 추가

    status = pthread_sigmask(SIG_BLOCK, &sigset, NULL); // 호출 쓰레드의 시그널 마스크 변경-> 해당 함수가 함수를 호출한 쓰레드에서만 동작
                                                        // 시그널 마스크 : 특정 쓰레드가 어떤 시그널을 차단하거나 수신할 수 없도록 설정
                                                        // 현재 쓰레드가 특정 시그널을 처리하지 못하도록 블록
    if (status != 0) {
        fprintf(stderr, "Set signal mask failed\n");
        exit(1);
    }

    status = pthread_create(&tid, NULL, signal_thread, NULL);
    if (status != 0) {
        fprintf(stderr, "Create thread failed: %d\n", status);
        exit(1);
    }

    while (1) {
        sleep(arg);
        pthread_mutex_lock(&mutex); // 뮤텍스를 잠근 후 메인 쓰레드가 임계 영역에 진입 
        printf("Main thread: mutex locked\n");
        if (completed) { // completed == 1 이면, 시그널 쓰레드가 SIGINT 신호를 3번 처리하고 작업 완료 
            pthread_mutex_unlock(&mutex); // 완료되면 뮤텍스 해제 
            break;
        }
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제 
    }

    // 뮤텍스를 잠근다 : 특정 쓰레드가 공유 자원에 접근하는것
    //                : 다른 쓰레드가 같은 뮤텍스를 잠그려고 하면, 해당 뮤텍스가 해제될 때까지 대기
    // 뮤텍스를 해제하다 : 현재 뮤텍스를 소유한 쓰레드가 작업을 마치고 잠금을 푸는 것것

    pthread_join(tid, NULL);
    printf("Main thread: completed\n");
    return 0;
}

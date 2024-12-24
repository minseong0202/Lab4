#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CHILD 5
#define BUFFER_SIZE 1024

// 공유 자원
char message_buffer[BUFFER_SIZE];
int message_ready = 0; // 메시지가 준비되었는지 표시
int active_child = 1; // 현재 입력 중인 클라이언트
int program_running = 1; // 프로그램 상태를 관리

// 동기화를 위한 뮤텍스와 조건 변수
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

// 부모 쓰레드 (서버 역할)
void *parents_thread(void *arg) {
    while (program_running) {
        pthread_mutex_lock(&mutex); // 뮤텍스를 잠근다.

        // 자식 쓰레드 메시지가 준비될 때까지 대기
        while (!message_ready && program_running) { // message_ready가 0이면  
            pthread_cond_wait(&cond_var, &mutex); // message_ready가 1이 될 때까지 대기
        }

        if (!program_running) { // 프로그램이 종료되었을 때 
            pthread_mutex_unlock(&mutex);
            break;
        }

        // 메시지 방송
        printf("부모쓰레드가 자식쓰레드 %d [%s] 요청을 받았다.\n", active_child, message_buffer);

        // 메시지 상태 초기화
        // 메시지가 처리되었으므로 자식쓰레드가 다시 입력할 수 있게 허용 
        message_ready = 0;


        // 다음 클라이언트로 이동
        active_child = (active_child % MAX_CHILD) + 1;
        pthread_cond_broadcast(&cond_var); // 모든 자식쓰레드에게 신호 전달
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제하여 다른쓰레드 접근 가능하도록 함 
    }

    return NULL;
}

// 자식 쓰레드 (클라이언트 역할)
void *child_thread(void *arg) {
    int child_id = *(int *)arg; 
    char input_buffer[BUFFER_SIZE];

    while (program_running) {
        pthread_mutex_lock(&mutex);

        // 현재 쓰레드가 입력할 차례인지 확인
        while (active_child != child_id && program_running) {
            pthread_cond_wait(&cond_var, &mutex); // 대기 
        }

        if (!program_running) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // 사용자 입력 받기
        printf("자식쓰레드 %d 입력: ", child_id);
        fgets(input_buffer, BUFFER_SIZE, stdin);
        input_buffer[strcspn(input_buffer, "\n")] = '\0'; // 개행 문자 제거

        // 종료 조건 처리
        if (strcmp(input_buffer, "exit") == 0) {
            program_running = 0;
            pthread_cond_broadcast(&cond_var); // 모든 쓰레드 깨우기
            pthread_mutex_unlock(&mutex); //뮤텍스 해제
            break;
        }

        // 메시지를 부모 쓰레드로 전달
        snprintf(message_buffer, BUFFER_SIZE, "%s", input_buffer);
        message_ready = 1;

        // 부모 쓰레드에 신호 보내기
        pthread_cond_signal(&cond_var);
        pthread_mutex_unlock(&mutex);

        // 입력이 끝난 후 잠시 대기
        sleep(1);
    }

    printf("자식쓰레드 %d 종료\n", child_id);
    return NULL;
}


int main() {
    pthread_t child[MAX_CHILD];
    pthread_t parents;
    int child_id[MAX_CHILD];

    // 클라이언트 ID 초기화
    for (int i = 0; i < MAX_CHILD; i++) {
        child_id[i] = i + 1;
    }

    // 부모 쓰레드 생성
    pthread_create(&parents, NULL, parents_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CHILD; i++) {
        pthread_create(&child[i], NULL, child_thread, (void *)&child_id[i]);
    }

    // 클라이언트 쓰레드 종료 대기
    for (int i = 0; i < MAX_CHILD; i++) {
        pthread_join(child[i], NULL);
    }

    // 부모 쓰레드 종료
    pthread_join(parents, NULL);

    // 자원 정리
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    printf("프로그램 종료\n");
    return 0;
}

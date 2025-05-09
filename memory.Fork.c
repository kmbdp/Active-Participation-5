#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);

    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    *shared_memory = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child modifying shared memory...\n");
        *shared_memory = 42;
        printf("Child updated shared memory to: %d\n", *shared_memory);
    } else {
        sleep(1);
        printf("Parent reads shared memory: %d\n", *shared_memory);
    }

    if (shmdt(shared_memory) < 0) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            perror("shmctl");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

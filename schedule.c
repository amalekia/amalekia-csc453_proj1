#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_PROCESSES 100
#define MAX_ARGS 10

struct Queue {
    int front, rear, size;
    unsigned capacity;
    char** array;
};

struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (char**) malloc(queue->capacity * (sizeof(char*)));
    return queue;
}

void dequeue(struct Queue* queue) {
    if (queue->size == 0) {
        perror("error dequeuing from empty queue\n");
    }
    char* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
}

void enqueue(struct Queue* queue, char* item) {
    if (queue->size == queue->capacity) {
        perror("error enqueuing to full queue\n");
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int quantum = atoi(argv[0]);

    char *processArgs[MAX_ARGS];
    for (int i = 0; i < MAX_ARGS; i++) {
        processArgs[i] = (char*) malloc(20 * sizeof(char));
    }
    int processArgsIndex = 0;

    struct Queue* processQueue = createQueue(MAX_PROCESSES);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], ":") == 0) {
            enqueue(processQueue, processArgs[processArgsIndex]);
            processArgsIndex = 0;
        }
        else {
            strcpy(processArgs[processArgsIndex], argv[i]);
            processArgsIndex++;
        }
    }

    //scheduling and executing processes
    pid_t *childlist[MAX_PROCESSES];
    pid_t childpid;
    int sizeChildList = 0;

    int i = 0;
    while ((childpid = fork()) != 0 && i < MAX_PROCESSES) {
        if (childpid == 0) {
            raise(SIGSTOP);
            execvp(processQueue->front, args);
            perror("");
            exit(1);
        }
        else {
            childlist[i] = childpid;
            sizeChildList++;
        }
        i++;
    }

    for (int i = 0; i < sizeChildList; i++) {
        struct itimerval timer;
        timer.it_interval.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_value.tv_sec = quantum / 1000;
        timer.it_value.tv_usec = 0;

        kill(childlist[i], SIGCONT);
        if(setitimer(ITIMER_REAL, &timer, NULL) == SIGALRM){
            kill(pidlist[i], SIGSTOP);
        }
        kill(pidlist[i], SIGSTOP);
    }

    free(processQueue);
    free(processArgs);
    return 0;
}
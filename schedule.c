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
    char*** array;
};

struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (char***) malloc(queue->capacity * sizeof(char**));
    return queue;
}

char** dequeue(struct Queue* queue) {
    if (queue->size == 0) {
        perror("error dequeuing from empty queue\n");
        return NULL;
    }
    char** item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void enqueue(struct Queue* queue, char** item) {
    if (queue->size == queue->capacity) {
        perror("error enqueuing to full queue\n");
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    //int quantum = atoi(argv[0]);
    
    // Parsing command line arguments and grouping them into arrays
    struct Queue* queue = createQueue(MAX_PROCESSES);
    char *argArray[MAX_ARGS];
    printf("%d", argc);

    int argCount = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], ":") == 0) {
            printf("gothere");
            argArray[argCount] = NULL; // Mark end of argument list
            char **newArgArray = (char **)malloc((argCount + 1) * sizeof(char*));
            memcpy(newArgArray, argArray, (argCount + 1) * sizeof(char*));
            enqueue(queue, newArgArray);
            argCount = 0; // Reset count for next group
        } else {
            argArray[argCount++] = argv[i];
        }
    }
    if (argCount > 0) {
        argArray[argCount] = NULL;
        char **newArgArray = (char **)malloc((argCount + 1) * sizeof(char*));
        memcpy(newArgArray, argArray, (argCount + 1) * sizeof(char*));
        enqueue(queue, newArgArray);
    }

    //scheduling and executing processes
    // pid_t childlist[MAX_PROCESSES];
    // pid_t childpid;
    // int sizeChildList = 0;

    // int i = 0;
    // while ((childpid = fork()) != 0 && i < MAX_PROCESSES) {
    //     if (childpid == 0) {
    //         raise(SIGSTOP);
    //         execvp(queue->front, args);
    //         perror("error when executing process\n");
    //         exit(1);
    //     }
    //     else {
    //         childlist[i] = childpid;
    //         sizeChildList++;
    //     }
    //     i++;
    // }

    // for (int i = 0; i < sizeChildList; i++) {
    //     struct itimerval timer;
    //     timer.it_interval.tv_usec = 0;
    //     timer.it_interval.tv_sec = 0;
    //     timer.it_value.tv_sec = quantum / 1000;
    //     timer.it_value.tv_usec = 0;

    //     kill(childlist[i], SIGCONT);
    //     if(setitimer(ITIMER_REAL, &timer, NULL) == SIGALRM){   // halts the child based on the time quantum
    //         kill(childlist[i], SIGSTOP);     
    //     }
    //     kill(childlist[i], SIGSTOP);
    // }

    free(queue->array);
    free(queue);
    return 0;
}
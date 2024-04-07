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

typedef struct Node {
    char** args;
    char* funcname;
    pid_t pid;
    struct Node* next;
} Node;

typedef struct Queue {
    Node *front, *rear;
    unsigned capacity;
} Queue;

Node* createNode(char **stringList, int pid, char *name) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newNode->args = stringList;
    newNode->pid = pid;
    newNode->funcname = strdup(name);
    newNode->next = NULL;
    return newNode;
}

Queue* createQueue() {
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Queue *queue, Node *newNode) {
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

Node* dequeue(Queue *queue) {
    if (queue->front == NULL) {
        printf("Queue is empty.\n");
        return NULL;
    }

    Node *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return temp;
}

void freeQueue(Queue *queue) {
    while (queue->front != NULL) {
        Node *temp = dequeue(queue);
        free(temp->funcname);
        free(temp->args);
        free(temp);
    }
    free(queue);
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int quantum = atoi(argv[1]);
    
    // Parsing command line arguments and grouping them into arrays
    struct Queue* queue = createQueue(MAX_PROCESSES);
    char *argArray[MAX_ARGS];

    int argCount = 0;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], ":") == 0) {
            argArray[argCount] = NULL; // Mark end of argument list
            char **newArgArray = (char **)malloc((argCount + 1) * sizeof(char*));
            memcpy(newArgArray, argArray, (argCount + 1) * sizeof(char*));
            enqueue(queue, createNode(newArgArray, 0, newArgArray[0]));
            argCount = 0; // Reset count for next group
        } else {
            argArray[argCount++] = argv[i];
        }
    }
    if (argCount > 0) {
        argArray[argCount] = NULL;
        char **newArgArray = (char **)malloc((argCount + 1) * sizeof(char*));
        memcpy(newArgArray, argArray, (argCount + 1) * sizeof(char*));
        enqueue(queue, createNode(newArgArray, 0, newArgArray[0]));
    }
    printf("Time Quantum: %d\n", quantum);
    printf("First element: %s\n", queue->front->funcname);

    //scheduling and executing processes
    // pid_t childlist[MAX_PROCESSES];
    // pid_t childpid;
    // int sizeChildList = 0;

    // int i = 0;
    // while ((childpid = fork()) != 0 && i < queue->size) {
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

    //frees pointer to queue
    freeQueue(queue);
    return 0;
}
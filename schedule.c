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
    int status;
    struct Node* next;
} Node;

typedef struct Queue {
    Node *front, *rear;
    unsigned capacity;
} Queue;

pid_t childpid;
int status;

//creates a new process (Node*) with the given arguments
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

//creates a new queue
Queue* createQueue() {
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    queue->front = queue->rear = NULL;
    queue->capacity = MAX_PROCESSES;
    return queue;
}

//enqueues a new process (Node*) into the queue
void enqueue(Queue *queue, Node *newNode) {
    if (queue->rear != NULL && queue->capacity == 0) {
        printf("Queue is full.\n");
        return;
    }
    else {
        if (queue->rear == NULL) {
            queue->front = queue->rear = newNode;
        } else {
            queue->rear->next = newNode;
            queue->rear = newNode;
        }
        queue->capacity--;
    }
}

//dequeues a process (Node*) from the queue and returns it
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

//frees entire queue and all nodes
void freeQueue(Queue *queue) {
    while (queue->front != NULL) {
        Node *temp = dequeue(queue);
        free(temp->funcname);
        free(temp->args);
        free(temp);
    }
    free(queue);
}

//signal handling functions
void handle_alrm_action(){
    printf("Timer expired. Killing process\n");
    kill(childpid, SIGSTOP);
}

void handle_chld_action(){
    //set a new timer and set it to 0 ms
    struct itimerval timer;
    timer.it_interval.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

//main program (scheduling function)
int main(int argc, char* argv[]) {
    
    // Parse command line arguments
    int quantum = atoi(argv[1]);
    
    // Parsing command line arguments and grouping them into arrays
    struct Queue* queue = createQueue();
    char *argArray[MAX_ARGS];

    int argCount = 0;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], ":") == 0) {
            argArray[argCount] = NULL; // Mark end of argument list
            char **newArgArray = (char **)malloc((argCount + 1) * sizeof(char*));
            memcpy(newArgArray, argArray, (argCount + 1) * sizeof(char*));
            enqueue(queue, createNode(newArgArray, 0, newArgArray[0]));
            argCount = 0; // Reset count for next group
        } 
        else {
            if (argCount == MAX_ARGS + 1) {
                perror("Too many arguments\n");
                exit(EXIT_FAILURE);
            }
            else {
                argArray[argCount++] = argv[i];
            }
        }
    }
    if (argCount > 0) {
        argArray[argCount] = NULL;
        char **newArgArray = (char **)malloc((argCount + 1) * sizeof(char*));
        memcpy(newArgArray, argArray, (argCount + 1) * sizeof(char*));
        enqueue(queue, createNode(newArgArray, 0, newArgArray[0]));
    }

    //scheduling and executing processes

    Node* curr = queue->front;
    while (curr != NULL) {
        childpid = fork();

        if (childpid == 0) {
            raise(SIGSTOP);
            execvp(strcat("./", curr->funcname), &(curr->args[1]));
            perror("error when executing process\n");
            exit(1);
        }
        else {
            curr->pid = childpid;
        }
        curr = curr->next;
    }

    if (childpid > 0) {
        // Set up the signal handlers in the parent process
        signal(SIGALRM, handle_alrm_action);
        signal(SIGCHLD, handle_chld_action);

        while (queue->front != NULL) { 
            struct itimerval timer;
            timer.it_interval.tv_usec = 0;
            timer.it_interval.tv_sec = 0;
            timer.it_value.tv_sec = quantum / 1000;
            timer.it_value.tv_usec = 0;

            //dequeues a process from queue and starts that process along with the timer
            Node* process = dequeue(queue);
            childpid = process->pid;
            printf("%d\n", childpid);
            kill(childpid, SIGCONT);
            setitimer(ITIMER_REAL, &timer, NULL);

            if (waitpid(process->pid, &status, WNOHANG) == -1) {
                perror("Error waiting for child process\n");
                exit(EXIT_FAILURE);
            }
            else {
                if (WIFEXITED(status)) {
                    //child terminated normally
                    free(process->args);
                    free(process->funcname);  
                    free(process);
                }
                else {
                    //enqueues the process back into the queue
                    enqueue(queue, process);
                }
            }
        }
    }

    //frees pointer to queue
    freeQueue(queue);
    return 0;
}
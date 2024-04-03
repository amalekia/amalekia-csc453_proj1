#include 

int main(char argc, char* argv[]) {
    // Parse command line arguments
    if (strcmp(argv[0], "schedule") != 0) {
            printf("error: invalid command\n");
            return 0;
    }

    int quantum = argv[1];

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], ":") == 0) {
            printf("Usage: schedule [schedule file]\n");
            return 0;
        }
    }

    // Read in the schedule file
    // Create a schedule object
    // Print the schedule
    // Return 0
}
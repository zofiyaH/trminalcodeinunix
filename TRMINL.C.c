#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/wait.h> 

int main() {
    char buf[1024]; 
    char *path;
    char *libraries[50]; 
    int i = 0;

    printf("\n######################################################\n");
    printf("Welcome to your Shell!\nEnter commands or 'leave'/'Leave' to exit.\n");
    printf("######################################################\n");

    while (1) {
        printf("student@lubu:~$ ");
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("Error, can't read command.\n");
            continue;
        }
        
        buf[strcspn(buf, "\n")] = '\0'; // מסיר תו ירידת שורה

        if (strcmp(buf, "leave") == 0 || strcmp(buf, "Leave") == 0) {
            break;
        }
        
        path = strdup(getenv("PATH"));
        if (path == NULL) {
            printf("Error: couldn't allocate memory for PATH.\n");
            continue;
        }
        
        i = 0;
        char *cwd = strtok(path, ":");
        while (cwd != NULL && i < 50) { // הגדלת גבול הלולאה למנוע חריגה
            libraries[i++] = cwd;
            cwd = strtok(NULL, ":");
        }
        libraries[i] = NULL; 
        
        pid_t pid = fork();
        if (pid == -1) {
            printf("Error: fork failed.\n");
            free(path);
            continue;
        }

        if (pid == 0) { // תהליך הבן
            char command[1024];
            char *argv[] = {buf, NULL};

            for (int j = 0; libraries[j] != NULL; j++) {
                if (snprintf(command, sizeof(command), "%s/%s", libraries[j], buf) >= sizeof(command)) {
                    printf("Error: command string too long.\n");
                    free(path);
                    exit(1);
                }
                execv(command, argv);
            }

            printf("Sorry, command not found: %s\n", buf);
            free(path);
            exit(1);
        } else { // תהליך האב
            int status;
            wait(&status);
            free(path);
        }
    }

    printf("Goodbye\n");
    return 0;
}
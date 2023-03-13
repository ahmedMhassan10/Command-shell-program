#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#define MAX 1024
#define Value 2048

char * word[MAX];
char intialWord[MAX];
char* doubleWord[2];
int len =0;

int pid;
bool myWait;
int s;

void myParse() {
    len = 0;
    if (intialWord[strlen(intialWord) - 1] == '&') {
        intialWord[strlen(intialWord) - 1] = '\0';
        myWait = false;
    }
    int index = 0;
    word[strlen(intialWord) - 1] = '\0';
    word[0] = strtok(intialWord, " \n");
    if (word[index] == NULL)
        return ;
    while (word[index] != NULL) {
        len++;
        word[++index] = strtok(NULL, " \n");
    }
    bool nextRound = false;
    int index2 = 1;
    while (word[index2] != NULL) {
        if (word[index2][0] == '$') {
            char *val = word[index2];
            if(val[strlen(val)-1]=='"'){
                val[strlen(val)-1]='\0';
            }
            nextRound = true;
            val = val + 1;
            word[index2] = getenv(val);
        }
        index2++;
    }
    if (nextRound) {
        int index3 = 0;
        int length = 0;
        while (word[index3] != NULL) {
            for (int j = 0; j < strlen(word[index3]); ++j) {
                intialWord[length++] = word[index3][j];
            }
            index3++;
            intialWord[length++]='\n';
        }
        intialWord[length-1] = '\0';
        myParse();
    }
}
void dir() {
    char temp[Value];
    if (getcwd(temp, sizeof(temp)) == NULL) {
        printf("There is bug here");
    } else {
        printf("\n %s--> Your command is ==> ", temp);
    }
}

void cDir() {
    if (word[1] == NULL||word[1][0]=='~') {
        chdir(getenv("HOME"));
    } else {
        chdir(word[1]);
    }
    dir();
}
void myRead(){
    dir();
    fgets(intialWord, MAX, stdin);
    intialWord[strlen(intialWord)-1]='\0';
    myParse();
}
void echo() {
    if (word[1][0]!=NULL) {
        int i = 1;
        word[1] = strtok(word[1], "\"");
        if (len > 2)
            word[len - 1][strlen(word[len - 1])] = '\0';
        while (word[i] != NULL) {
            printf("%s ", word[i]);
            i++;
        }
    }
}


void export() {
    char *var = word[1];
    doubleWord[0] = strtok(word[1], "=");
    var = var + strlen(doubleWord[0]) + 1;
    doubleWord[1] = var;
    if (doubleWord[1][0] == '\"') {
        doubleWord[1] = strtok(doubleWord[1], "\"");
    }
    if (len > 2)
        word[len - 1][strlen(word[len - 1])] = '\0';
    int i = 2;
    while (word[i++] != NULL) {
        memset(doubleWord[1] + strlen(doubleWord[1]), ' ', 1);
    }
    if(doubleWord[1][strlen(doubleWord[1])-1]=='"')
        doubleWord[1][strlen(doubleWord[1])-1]='\0';
    setenv(doubleWord[0], doubleWord[1], 1);
}
void executeCommand() {
    if (strcmp( "exit",word[0]) == 0) {
        exit(0);
    }
    pid = fork();
    if (!pid) {
        int x = execvp(word[0], word);
        if (x < 0) {
            perror("Command can't be executed \n");
            exit(x);
        }
        exit(0);
    } else {
        if (myWait) {
            waitpid(pid, &s, WUNTRACED);
        }
    }
}

void log()
{
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}

    FILE* fptr;
    fptr = fopen("log.txt", "a");

    fputs("termination of child\n",fptr);
    fclose(fptr);
}

int main() {
      signal (SIGCHLD, log);
    while (1) {
        myWait = true;
        myRead();
        if (!strcmp(word[0], "cd") || !strcmp(word[0], "echo") || !strcmp(word[0], "export")) {
            if (!strcmp(word[0], "cd")) {
                cDir();
            } else if (!strcmp(word[0], "echo")) {
                echo();
            } else if (!strcmp(word[0], "export")) {
                export();
            }
        } else {
            executeCommand();
        }
    }
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void clean_data(char *data);

int main(int argc, char *argv[])
{
    int fd;

    //fifo file path
    char myfifo[] = "/tmp/myfifo";
    //create named fifo
    mkfifo(myfifo, 0666);
    //create two char arrays for control our arguments
    char arg1[100] = {'\0'};
    char arg2[100] = {'\0'};

    while (1)
    {
        //take an argument from user with fgets()
        printf("Lutfen bir sorgu giriniz\n");
        fgets(arg2, 100, stdin);

        // opening fifo with write only mode
        fd = open(myfifo, O_WRONLY);
        // write fifo user inputs from arg2 array
        write(fd, arg2, strlen(arg2) + 1);
        close(fd);

        //open fifo with readonly mode
        fd = open(myfifo, O_RDONLY);
        //read database values from fifo on arg1 array
        read(fd, arg1, sizeof(arg1));
        close(fd);

        //check values of arg1 with null value
        printf("%s\n", arg1);
        if (strcmp("null", arg1) == 0 || strcmp("Yanlis ya da eksik komut girdiniz", arg1) == 0 ||strcmp("Numara olmayan bir deger tespit edildi...", arg1) == 0 || strcmp("isim olmayan bir deger tespit edildi...", arg1)==0 
            ||strcmp("file could not be opened", arg1) == 0 )
        { 
            //if is null continue the program 
            printf("\n");
            continue;
        }

        //second part of main method writing database data on a txr file
        //taking input from user for save data on txt file
        printf("Sorgu sonucunu kaydetmek ister misiniz?(e/h)\n");
        
        fgets(arg2, 100, stdin);
        clean_data(arg2);
        
        if (strcmp("e", arg2) == 0)
        {
            int pipefd[2];
            //check pipe
            if (pipe(pipefd) < 0)
            {
                perror("pipe");
                exit(1);
            }
            int pid = fork();
            int child;
            //if there is a fork
            if (pid == 0)
            {   //write on unnamedpipe
                write(pipefd[1], arg1, strlen(arg1) + 1);
                child = execv("kaydet", NULL);
                //and close pipe
                close(pipefd[0]);
                close(pipefd[1]);
                exit(0);
            }
            else
            {   //wait for child
                wait(&child);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            printf("Bilgiler kaydedildi\n");
        }
        else if (strcmp("h", arg2) == 0)
        {
            printf("Bilgiler kaydedilmedi.\n");
        }
    }
    return 0;
}

void clean_data(char *data)
{   //while en of the string
    while (*data != '\0')
    {
        if (*data == '\n')
        {
            //if there is a '\n' char , clean data
            *data = '\0';
            break;
        }
        data++;
        //increase data pointer.
    }
}
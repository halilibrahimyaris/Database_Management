#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void writeToFile(char *data);

int main(int argc, char *argv[])
{
    //create an array
    char output[100] = {'\0'};

    read(3, output, 100);
    //call write file function
    writeToFile(output);
    return 0;
}

void writeToFile(char *data)
{
    FILE *file;
    //open a file with append mode
    file = fopen("output.txt", "a");
    if (file == NULL)
    {
        //if file is NULL return this message
        printf("output.txt file could not be opened");
    }
    else
    {
        if (strlen(data) > 0)
        {   ///else write on the file with fputs
            fputs(data, file);
        }
        //and close file
        fclose(file);
    }
}
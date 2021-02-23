#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXCHAR 100
#define MAXLIST 10
#define SQLLEN 7

//define a struct for a better control string values
typedef struct
{
    char name[100];
    char num[10];
} info;

void executeSql(char query[MAXLIST][MAXCHAR], info infos, char *sqlData);
void parseValue(char *str, char parsed[MAXLIST][MAXCHAR], char *con, int index);
void removeLines(char *data);
void clearInfo(char *sql_data);
void clearArr(char arr[MAXLIST][MAXCHAR]);
int isEmpty(char *str);
void printArgs(char args[MAXLIST][MAXCHAR]);
int arrLen(char str[MAXLIST][MAXCHAR]);

int main(int argc, char *argv[])
{ //this array defined for keeping our sql values
    char *sqlValues[SQLLEN] = {"select", "*", "name", "number", "from", "where", "="};
    char sql[MAXCHAR] = {'\0'};
    char parsedSql[MAXLIST][MAXCHAR] = {'\0'};
    char sqlData[MAXCHAR] = {'\0'};
    info infos;

    int fd1;
    //fifo file path
    char myfifo[] = "/tmp/myfifo";
    //create named fifo
    mkfifo(myfifo, 0666);

    while (1)
    {
        //open fifo with readonly mode
        fd1 = open(myfifo, O_RDONLY);
        //read database values from fifo on sql array
        read(fd1, sql, MAXCHAR);
        //close named pipe
        close(fd1);

        removeLines(sql);
        //parsed to space char our sql commands
        parseValue(sql, parsedSql, " ", 0);
        //takes its length
        int lenght = arrLen(parsedSql);
        //parse to = char our sql command
        parseValue(parsedSql[lenght - 1], parsedSql, "=", lenght - 1);

        printf("SQL query :  Length: %d \n", arrLen(parsedSql));

        //print sql commands
        printArgs(parsedSql);
        // send parsed sql array to  execquery function
        executeSQL(parsedSql, infos, sqlData);
        // opening fifo with write only mode

        fd1 = open(myfifo, O_WRONLY);
        // write fifo user inputs from sqlDataarray
        write(fd1, sqlData, strlen(sqlData) + 1);
        close(fd1);
        clearInfo(sqlData);
        clearArr(parsedSql);
    }
    return 0;
}

void removeLines(char *data)
{ //while en of the string
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
void parseValue(char *data, char parsed[MAXLIST][MAXCHAR], char *con, int index)
{
    //this function split datas for given condition.
    char *d;
    while ((d = strsep(&data, con)) != NULL)
    {
        if (isEmpty(d))
        {
            continue;
        }
        strcpy(parsed[index], d);
        index++;
    }
}

void clearInfo(char *sql_data)
{ // this function clear datas in a char
    while (*sql_data != '\0')
    {
        *sql_data = '\0';
        sql_data++;
    }
}

void clearArr(char arr[MAXLIST][MAXCHAR])
{
    //this function clears char array's element
    int i = 0;
    while (strcmp(arr[i], "\0") != 0)
    { //we call our clear Info for every element
        clearInfo(arr[i]);
        i++;
        //increase file
    }
}
int isEmpty(char *str)
{
    //this function is controls a string is null or not
    while (*str != '\0')
    {
        if (*str != ' ')
        {
            return 0;
        }
        str++;
    }
    return 1;
}

void printArgs(char args[MAXLIST][MAXCHAR])
{ // this method print our sql values
    int i = 0;
    while (strcmp(args[i], "\0") != 0)
    {
        printf("%s\n", args[i]);
        i++;
    }
}
int arrLen(char str[MAXLIST][MAXCHAR])
{
    //this method gives an array length
    int i = 0;
    while (strcmp(str[i], "\0") != 0)
    {
        i++;
    }
    return i;
}

void executeSQL(char sql_data[MAXLIST][MAXCHAR], info infos, char *result)
{ //create a file pointer fp for file operations
    FILE *fp;
    char input[100] = {"\0"};
    int i = 0;
    // an integer value for null control
    int isThere = 0;
    // a control for null file or not exist files
    if ((fp = fopen(sql_data[3], "r")) == NULL)
    {
        strcat(result, "file could not be opened");
    }
    // a general control for sql command format
    else if (strcmp(sql_data[0], "select") != 0 || strcmp(sql_data[2], "from") != 0 || strcmp(sql_data[4], "where") != 0)
    {
        //return a error message with named pipe
        strcat(result, "Yanlis ya da eksik komut girdiniz");
    }
    //check integer value for searching on number attribute.
    else if ((strcmp(sql_data[5], "number") == 0) && !isNumber(sql_data[6]))
    {
        strcat(result, "Numara olmayan bir deger tespit edildi...");
    }
    else if ((strcmp(sql_data[5], "name") == 0) && isNumber(sql_data[6]))
    {
        strcat(result, "isim olmayan bir deger tespit edildi...");
    }
    else
    { //read file while end of file
        while (!feof(fp))
        { //take inputs with fgets
            fgets(input, 100, fp);
            sscanf(input, "%s %s\n", infos.name, infos.num);
            //and write these values on a struct which infos
            if ((strcmp(sql_data[5], "name")) == 0)
            { //check sql input name or number
                if ((strcmp(sql_data[6], infos.name)) == 0)
                { //and search it in struct
                    isThere = 1;
                    // if is there
                    printf("%s %s\n", infos.name, infos.num);
                    if ((strcmp(sql_data[1], "*")) == 0)
                    { //if sql data's first indexed element is equal *
                        strcat(result, input);
                        // return num and name
                    }
                    if ((strcmp(sql_data[1], "number")) == 0)
                    { //return only num
                        strcat(result, infos.num);
                        strcat(result, "\n");
                    }
                    if ((strcmp(sql_data[1], "name")) == 0)
                    { //return only name
                        strcat(result, infos.name);
                        strcat(result, "\n");
                    }
                }
            }
            else if ((strcmp(sql_data[5], "number")) == 0)
            { //check sql input name or number

                if ((strcmp(sql_data[6], infos.num)) == 0)
                { //if is exists
                    isThere = 1;
                    printf("%s %s\n", infos.name, infos.num);
                    if ((strcmp(sql_data[1], "*")) == 0)
                    { //if sql data's first indexed element is equal *
                        strcat(result, input);
                        // return num and name
                    }
                    if ((strcmp(sql_data[1], "number")) == 0)
                    { //return only num
                        strcat(result, infos.num);
                        strcat(result, "\n");
                    }
                    if ((strcmp(sql_data[1], "name")) == 0)
                    { //return name
                        strcat(result, infos.name);
                        strcat(result, "\n");
                    }
                }
            }
        }
        //close file
        fclose(fp);

        if (!isThere)
        { // if is not exist in our files
            strcat(result, "null");
            //return string null
        }
    }
}
int isNumber(char a[])
{
    int i;
    int length = 0;
    length = strlen(a);
    for (i = 0; i < length; i++)
    {
        //Girilen parametrenin her bir karakteri için isdigit() kontrol edilir.
        if (!isdigit(a[i]))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define BUFF_SIZE 2


// Written: Kevin Kim <kekim@hmc.edu>


void readSTDIN(int flag_i, int flag_l, int flag_q, int flag_n, char *input);
void readFile(char *filename, int flag_i, int flag_l, int flag_q, int flag_n, char *input);
void print(char *filename, int flag_i, int flag_l, int flag_q, int flag_n, int lineNum);
char *lineBuf;
char *chunk;
int chunkSize = 0;
int bufSize = BUFF_SIZE;
int exitcode = 0;
int main(int argc, char *argv[])
{
    lineBuf = (char *)malloc(sizeof(char) * 6);
    chunk = (char *)malloc(sizeof(char) * 3);

    int flag_i = 0;
    int flag_l = 0;
    int flag_n = 0;
    int flag_q = 0;
    int index = 0;
    int n = 0;
    FILE *fp;
    char *input;

    int i = 1;
    char *filename;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'i':
                flag_i = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case 'n':
                flag_n = 1;
                break;
            case 'q':
                flag_q = 1;
                break;
            default:
                break;
            }
            i += 1;
        }
        else
        {
            //if token encountered not a flag, break.
            break;
        }
    }

    //clever combinational logic readjusting flags n and l
    flag_n = !flag_q && flag_n && !flag_l;
    flag_l = !flag_q && flag_l;

    if (i == argc) //if only flags are present
    {
        printf("Error: Please specify a string to FGREP for.");
        exitcode = 1;
    }
    else if (i == (argc - 1)) // there is only 1 more argument after flags
    {
        input = argv[i];
        readSTDIN(flag_i, flag_l, flag_q, flag_n, input);
        //std input code here
    }
    else //there are more than 1 argument after flags. This is your standard [string] [filename] args.
    {
        input = argv[i];
        i += 1;
        while (i < argc)
        {
            filename = argv[i];
            readFile(filename, flag_i, flag_l, flag_q, flag_n, input);
            i += 1;
        }
    }
    free(chunk);
    free(lineBuf);
    exit(exitcode);
}

void readFile(char *filename, int flag_i, int flag_l, int flag_q, int flag_n, char *input)
{
    int lineNum = 1;
    int found = 0;
    int n = strlen(input);
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Couldn't open \'%s\': No such file or directory", filename);
        exitcode = 1;
        return;
    }
    while (fgets(chunk, bufSize + 1, fp) != NULL && !(found && flag_l))
    {
        strcat(lineBuf, chunk);
        if (chunk[strlen(chunk) - 1] == '\n')
        { // if the fgets has reached a newline
            //search through the lineBuf for our key.
            for (int i = 0; i < (bufSize); i++)
            {
                int b = (flag_i) ? strncasecmp(&lineBuf[i], input, n) : (strncmp(&lineBuf[i], input, n));
                if (b == 0)
                {
                    found = 1;
                    print(filename, flag_i, flag_l, flag_q, flag_n, lineNum);
                    break; //found key so stop parsing.
                }
            }
            lineNum++;
            memset(lineBuf, 0, 2 * (bufSize + 1)); //clears the lineBuf
        }
        else
        { // if fgets has not reached newline -> lineBuf = "...\0"
            //expand buffers 
            bufSize = bufSize * 2;
            lineBuf = realloc(lineBuf, 2 * (bufSize + 1));
            chunk = realloc(chunk, (bufSize + 1));
        }
    }
    if (!(found && flag_l))//in the case that we have no newline statements, do a last parse
    {
        for (int i = 0; i < (bufSize); i++)
        {
            int b = (flag_i) ? strncasecmp(&lineBuf[i], input, n) : (strncmp(&lineBuf[i], input, n));
            if (b == 0)
            {
                print(filename, flag_i, flag_l, flag_q, flag_n, lineNum);
            }
        }
    }
    fclose(fp);
}

/**
 * print logic that differs by flags
 **/
void print(char *filename, int flag_i, int flag_l, int flag_q, int flag_n, int lineNum)
{
    if (flag_q)
    {
        return;
    }
    else if (flag_l == 1)
    {
        printf("%s", filename);
    }
    else if (flag_n == 1)
    {
        printf("%s:%d:%s", filename, lineNum, lineBuf);
    }
    else
    {
        printf("%s:%s", filename, lineBuf);
    }
}

/**
 * helper function to read from stdin instead of FILE
 **/
void readSTDIN(int flag_i, int flag_l, int flag_q, int flag_n, char *input)
{
    int lineNum = 1;
    int found = 0;
    int n = strlen(input);
    char *filename = "(standard input)";
    while ((fgets(chunk, bufSize + 1, stdin) != NULL) && !(found && flag_l))
    {
        strcat(lineBuf, chunk);
        if (chunk[strlen(chunk) - 1] == '\n')
        { // if the fgets has reached a newline
            //search through the lineBuf for our search string.
            for (int i = 0; i < (bufSize); i++)
            {
                int b = (flag_i) ? strncasecmp(&lineBuf[i], input, n) : (strncmp(&lineBuf[i], input, n));
                if (b == 0)
                {
                    found = 1;
                    print(filename, flag_i, flag_l, flag_q, flag_n, lineNum);
                    break;
                }
            }
            lineNum++;
            memset(lineBuf, 0, 2 * (bufSize + 1));
        }
        else
        { // if fgets has not reached newline -> lineBuf = "...\0"
            bufSize = bufSize * 2;
            lineBuf = realloc(lineBuf, 2 * (bufSize + 1));
            chunk = realloc(chunk, (bufSize + 1));
        }
    }
    if (!(found && flag_l))
    {
        for (int i = 0; i < (bufSize); i++)
        {
            int b = (flag_i) ? strncasecmp(&lineBuf[i], input, n) : (strncmp(&lineBuf[i], input, n));
            if (b == 0)
            {
                print(filename, flag_i, flag_l, flag_q, flag_n, lineNum);
            }
        }
    }
}

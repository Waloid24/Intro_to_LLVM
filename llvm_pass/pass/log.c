#include <stdio.h>

// void BbStartLogger(char *funcName, char *calleeName, long int baseBlockAddress)
void funcStartLogger(char *funcName, size_t *bbAddress)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    fprintf(logFile, "%s: %p\n", funcName, bbAddress);
    fclose(logFile);
}

// void funcEndLogger(char *funcName, int result, long int baseBlockAddress)
void funcEndLogger(char *funcName, size_t *bbAddress, int result)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    fprintf(logFile, "%s: %p {%d}\n", funcName, bbAddress, result);
    fclose(logFile);
}
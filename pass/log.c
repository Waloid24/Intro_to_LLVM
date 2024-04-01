#include <stdio.h>

// void BbStartLogger(char *func_name, char *calleeName, long int baseBlockAddress)
void funcStartLogger(char *func_name, size_t *bbAddress)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    fprintf(logFile, "%s %p\n", func_name, bbAddress);
    fclose(logFile);
}

// void funcEndLogger(char *func_name, int result, long int baseBlockAddress)
void funcEndLogger(char *func_name, size_t *bbAddress, int result)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    fprintf(logFile, "ret %d %s %p\n", result, func_name, bbAddress);
    fclose(logFile);
}
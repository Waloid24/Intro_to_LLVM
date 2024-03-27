#include <stdio.h>

void funcStartLogger(char *funcName)
{
    FILE *logFile = fopen("dyn_log_file.txt", "a");
    fprintf(logFile, "[LOG] Start function '%s'\n", funcName);
    fclose(logFile);
}

void funcEndLogger(char *funcName)
{
    FILE *logFile = fopen("dyn_log_file.txt", "a");
    // fprintf(logFile, "[LOG] End function '%s' {%d}\n", funcName, result);
    fprintf(logFile, "[LOG] End function '%s'\n", funcName);
    fclose(logFile);
}
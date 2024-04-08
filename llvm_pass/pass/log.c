#include <stdio.h>

// void BbStartLogger(char *func_name, char *calleeName, long int baseBlockAddress)
void funcStartLogger(char *func_name, size_t *bbAddress)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    fprintf(logFile, "%s %p\n", func_name, bbAddress);
    fclose(logFile);
}

void callLogger(size_t *bbAddress, char *callee_name, char *caller_name, long int val_ID, size_t num_instr)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    fprintf(logFile, "call %s -> %s %p %zu %ld\n", callee_name, caller_name, bbAddress, num_instr, val_ID);
    fclose(logFile);
}

// void funcEndLogger(char *func_name, size_t *bbAddress, int result, size_t *nextBbAddress, size_t num_instr)
void funcEndLogger(char *func_name, size_t *bbAddress, int result, size_t num_instr)
{
    FILE *logFile = fopen("vizualization/dynamic.txt", "a");
    // fprintf(logFile, "ret %s %p %zu %p %d\n", func_name, bbAddress, num_instr, nextBbAddress, result);
    fprintf(logFile, "ret %s %p %zu %d\n", func_name, bbAddress, num_instr, result);
    fclose(logFile);
}
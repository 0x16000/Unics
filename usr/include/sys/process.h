#ifndef SYS_PROCESS_H
#define SYS_PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_PROCESSES 32
#define MAX_PROCESS_NAME 32

typedef enum {
    PROCESS_UNUSED = -1,
    PROCESS_RUNNING,
    PROCESS_SLEEPING,
    PROCESS_STOPPED,
    PROCESS_ZOMBIE
} ProcessState;

typedef struct {
    int pid;
    char name[MAX_PROCESS_NAME];
    ProcessState state;
    int ppid;  // Parent process ID
    bool used;
} Process;

extern Process process_table[MAX_PROCESSES];

// Process management
void process_init(void);
int process_create(const char *name, int ppid);
int process_kill(int pid);
int process_cleanup(int pid);
Process *process_get(int pid);
int process_find(const char *name);
int process_count(void);
void process_list(void);

#endif // SYS_PROCESS_H

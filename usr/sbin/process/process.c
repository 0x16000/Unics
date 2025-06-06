#include <string.h>
#include <stdio.h>
#include <sys/process.h>

Process process_table[MAX_PROCESSES];

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].used = false;
    }
}

int process_create(const char *name, int ppid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!process_table[i].used) {
            process_table[i].pid = i + 100;  // PID starts at 100
            strncpy(process_table[i].name, name, MAX_PROCESS_NAME - 1);
            process_table[i].name[MAX_PROCESS_NAME - 1] = '\0';
            process_table[i].state = PROCESS_RUNNING;
            process_table[i].ppid = ppid;
            process_table[i].used = true;
            return process_table[i].pid;
        }
    }
    return -1;  // No space left
}

int process_kill(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && process_table[i].pid == pid) {
            process_table[i].used = false;
            return 0;  // Success
        }
    }
    return -1;  // Not found
}

Process *process_get(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    return NULL;
}

void process_list(void) {
    printf("PID   PPID  STATE     CMD\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used) {
            const char *state_str;
            switch (process_table[i].state) {
                case PROCESS_RUNNING:  state_str = "RUNNING"; break;
                case PROCESS_SLEEPING: state_str = "SLEEPING"; break;
                case PROCESS_STOPPED:  state_str = "STOPPED"; break;
                case PROCESS_ZOMBIE:   state_str = "ZOMBIE"; break;
                default:               state_str = "UNKNOWN"; break;
            }
            printf("%-5d %-5d %-9s %s\n", process_table[i].pid,
                   process_table[i].ppid, state_str, process_table[i].name);
        }
    }
}

#include <string.h>
#include <stdio.h>
#include <stdatomic.h>
#include <sys/process.h>

Process process_table[MAX_PROCESSES];
static atomic_int next_pid = 100;

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].used = false;
        process_table[i].state = PROCESS_UNUSED;
    }
}

int process_create(const char *name, int ppid) {
    if (!name || ppid < 0)
        return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!process_table[i].used) {
            Process *p = &process_table[i];

            p->pid = atomic_fetch_add(&next_pid, 1);
            strncpy(p->name, name, MAX_PROCESS_NAME - 1);
            p->name[MAX_PROCESS_NAME - 1] = '\0';
            p->state = PROCESS_RUNNING;
            p->ppid = ppid;
            p->used = true;

            return p->pid;
        }
    }
    return -1;
}

int process_kill(int pid) {
    if (pid < 0) return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && process_table[i].pid == pid) {
            process_table[i].state = PROCESS_ZOMBIE;
            return 0;
        }
    }
    return -1;
}

int process_cleanup(int pid) {
    if (pid < 0) return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        Process *p = &process_table[i];
        if (p->used && p->pid == pid && p->state == PROCESS_ZOMBIE) {
            memset(p, 0, sizeof(Process));
            p->used = false;
            p->state = PROCESS_UNUSED;
            return 0;
        }
    }
    return -1;
}

Process *process_get(int pid) {
    if (pid < 0) return NULL;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    return NULL;
}

int process_find(const char *name) {
    if (!name) return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && strcmp(process_table[i].name, name) == 0) {
            return process_table[i].pid;
        }
    }
    return -1;
}

int process_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used)
            count++;
    }
    return count;
}

static const char *state_to_string(ProcessState state) {
    switch (state) {
        case PROCESS_RUNNING:  return "RUNNING";
        case PROCESS_SLEEPING: return "SLEEPING";
        case PROCESS_STOPPED:  return "STOPPED";
        case PROCESS_ZOMBIE:   return "ZOMBIE";
        default:               return "UNUSED";
    }
}

void process_list(void) {
    printf("%-5s %-5s %-9s %s\n", "PID", "PPID", "STATE", "CMD");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used) {
            Process *p = &process_table[i];
            printf("%-5d %-5d %-9s %s\n",
                   p->pid, p->ppid, state_to_string(p->state), p->name);
        }
    }
}

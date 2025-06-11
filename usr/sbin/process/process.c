#include <string.h>
#include <stdio.h>
#include <sys/process.h>
#include <stdatomic.h>

/* Process table (declared in header as extern) */
Process process_table[MAX_PROCESSES];

/* Atomic PID counter to ensure unique PIDs across threads */
static atomic_int next_pid = 100;

/* Initialize the process table */
void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].used = false;
    }
}

/**
 * Create a new process entry
 * @param name Process name (will be truncated if too long)
 * @param ppid Parent process ID
 * @return PID of new process or -1 on error
 */
int process_create(const char *name, int ppid) {
    if (!name || ppid < 0) {
        return -1;
    }

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
    
    return -1; /* No free slots */
}

/**
 * Terminate a process
 * @param pid Process ID to terminate
 * @return 0 on success, -1 if process not found
 */
int process_kill(int pid) {
    if (pid < 0) return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && process_table[i].pid == pid) {
            Process *p = &process_table[i];
            
            /* Mark as zombie first to allow parent to check status */
            p->state = PROCESS_ZOMBIE;
            
            /* Parent notification would go here */
            
            return 0;
        }
    }
    
    return -1;
}

/**
 * Clean up a zombie process (to be called by parent)
 * @param pid Process ID to clean up
 * @return 0 on success, -1 if process not found or not a zombie
 */
int process_cleanup(int pid) {
    if (pid < 0) return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        Process *p = &process_table[i];
        if (p->used && p->pid == pid && p->state == PROCESS_ZOMBIE) {
            memset(p, 0, sizeof(Process));
            p->used = false;
            return 0;
        }
    }
    
    return -1;
}

/**
 * Get process information
 * @param pid Process ID to look up
 * @return Pointer to Process structure or NULL if not found
 */
Process *process_get(int pid) {
    if (pid < 0) return NULL;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    
    return NULL;
}

/**
 * List all active processes
 */
void process_list(void) {
    printf("%-5s %-5s %-9s %s\n", "PID", "PPID", "STATE", "CMD");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used) {
            Process *p = &process_table[i];
            
            const char *state_str = "UNKNOWN";
            switch (p->state) {
                case PROCESS_RUNNING:  state_str = "RUNNING"; break;
                case PROCESS_SLEEPING: state_str = "SLEEPING"; break;
                case PROCESS_STOPPED:  state_str = "STOPPED"; break;
                case PROCESS_ZOMBIE:   state_str = "ZOMBIE"; break;
            }
            
            printf("%-5d %-5d %-9s %s\n", 
                   p->pid, p->ppid, state_str, p->name);
        }
    }
}

/**
 * Find process by name
 * @param name Process name to search for
 * @return PID of first matching process or -1 if not found
 */
int process_find(const char *name) {
    if (!name) return -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].used && strcmp(process_table[i].name, name) == 0) {
            return process_table[i].pid;
        }
    }
    
    return -1;
}

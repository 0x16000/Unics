#include <sys/process.h>

int ps_main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    process_list();
    return 0;
}

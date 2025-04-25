#include <stdio.h>
#include <vers.h>

int whoami_main(int argc, char **argv) { 
	    (void)argc;  // Mark as unused if not needed
    	    (void)argv;  // Mark as unused if not needed
            printf(OS_HOSTNAME);
            printf("\n");
            return 0;
}

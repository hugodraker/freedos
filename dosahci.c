/*
 * ahci_main.c - FreeDOS AHCI Driver with INT 13h Support
 * Compile with: make && make install
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile int running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived shutdown signal...\n");
        running = 0;
        ahci_unload_driver();
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    printf("FreeDOS AHCI SATA Driver v1.0\n");
    printf("Support: INT 13h LBA28/LBA48, Extended Commands\n");
    printf("=====================================\n\n");
    
    /* Install signal handler for cleanup */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /* Install driver */
    int ret = ahci_install_driver();
    if (ret != 0) {
        printf("Driver installation failed with error %d\n", ret);
        return ret;
    }
    
    /* Keep driver loaded */
    printf("\nDriver active. Testing INT 13h...\n");
    printf("Run 'FDISK' or 'DIR' to verify drive access.\n");
    printf("Type 'EXIT' to shut down driver.\n\n");
    
    while (running) {
        sleep(1);
    }
    
    return 0;
}


; FreeDOS configuration with AHCI driver
LASTDRIVE=Z
FILES=40
BUFFERS=30

DEVICE=C:\FREDOS\HIMEM.SYS
DEVICEHIGH=C:\FREEDOS\AHCI.SYS
DOS=UMB,HIGH

SHELLHIGH=C:\FREDOS\COMMAND.COM C:\FREDOS /E:2048 /P

# Compile with DJGPP
i386-pc-msdosdjgpp-gcc -march=i386 -O2 -o ahci.sys ahci_main.c ahci_core.c \
    -Wl,--oformat=binary -nostartfiles

# Or for HX Extender
hxld -o ahci.exe ahci_main.c ahci_core.c -L/hx/lib -lx -lhx

# Copy to FreeDOS
cp ahci.sys /path/to/freedos/
echo "DEVICE=C:\\FREEDOS\\AHCI.SYS" >> /path/to/freedos/FDCONFIG.SYS


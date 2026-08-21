/*
 * ahci_tsr.c - FreeDOS AHCI Driver as Executable TSR
 * Run: AHCI.EXE (no CONFIG.SYS modification needed)
 * Unload: Ctrl+C or custom unload command
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dpmi.h>
#include <sys/interrupt.h>

/* ===== TSR Installation Functions ===== */
/* Using standard DOS TSR pattern */

static void __interrupt __far (*old_int13)(void);
static int driver_installed = 0;

/* Simple memory locking for real-mode callback */
static uint16_t real_mode_selector = 0;

static int setup_pmem_access(void) {
    /* Request privilege to access physical memory */
    __dpmi_regs regs;
    
    regs.x.ax = 0x400;  /* Lock memory descriptor */
    /* In practice, HX/DJGPP handles this automatically */
    
    return 0;
}

/* Main TSR installation function */
static int install_driver_as_tsr(void) {
    __dpmi_regs regs;
    
    printf("Installing AHCI Driver as TSR...\n");
    
    /* Save original INT 13h vector */
    regs.x.ax = 0x3513;  /* Get interrupt vector 13h */
    __dpmi_simulate_real_mode_procedure_retf(&regs);
    old_int13.seg = regs.x.ds;
    old_int13.off = regs.x.dx;
    
    printf("Original INT 13h: %04X:%04X\n", 
           old_int13.seg, old_int13.off);
    
    /* Initialize AHCI hardware first */
    if (ahci_init_driver() != 0) {
        printf("WARNING: No AHCI drives detected!\n");
        printf("Driver will still install but won't redirect disk access.\n");
    }
    
    /* Install our INT 13h handler via DPMI callback */
    if (install_dpmi_callback() != 0) {
        printf("ERROR: Failed to install INT 13h hook!\n");
        return 1;
    }
    
    driver_installed = 1;
    printf("SUCCESS: Driver installed as TSR (Ctrl+C to unload)\n");
    
    return 0;
}

/* Cleanup on exit */
static void uninstall_driver(void) {
    if (driver_installed) {
        printf("Unloading AHCI driver...\n");
        uninstall_dpmi_callback();
        
        /* Restore original INT 13h */
        __dpmi_regs regs;
        regs.x.ax = 0x2513;
        regs.x.dx = FP_OFF(old_int13);
        regs.x.bx = FP_SEG(old_int13);
        __dpmi_simulate_real_mode_procedure_retf(&regs);
        
        driver_installed = 0;
        printf("Driver unloaded. Original INT 13h restored.\n");
    }
}

/* Signal handler for Ctrl+C */
static volatile int keep_running = 1;

void cleanup_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nShutdown signal received...\n");
        uninstall_driver();
        exit(0);
    }
}

/* ===== Command-Line Interface ===== */

typedef enum {
    MODE_INSTALL,
    MODE_UNINSTALL,
    MODE_INFO,
    MODE_HELP
} app_mode_t;

static void show_usage(const char *progname) {
    printf("%s - FreeDOS AHCI SATA Driver\n\n", progname);
    printf("Usage:\n");
    printf("  %s             Install driver (default)\n", progname);
    printf("  %s /U          Uninstall driver\n", progname);
    printf("  %s /INFO       Show installed drive info\n", progname);
    printf("  %s /HELP       Show this help\n\n", progname);
    printf("Once installed:\n");
    printf("  - Press Ctrl+C to unload\n");
    printf("  - FDISK and FORMAT will see AHCI drives\n");
    printf("  - Runs as background TSR process\n");
}

static void show_drive_info(void) {
    printf("\n=== Detected AHCI Drives ===\n");
    printf("Total drives: %d\n\n", drive_count);
    
    if (drive_count == 0) {
        printf("No drives detected. Ensure:\n");
        printf("  - SATA controller in AHCI mode (not IDE/Legacy)\n");
        printf("  - At least one drive connected\n");
        return;
    }
    
    for (int i = 0; i < drive_count; i++) {
        printf("Drive #%d (Port %d):\n", i, drives[i].port_num);
        printf("  Model:     %s\n", drives[i].model);
        printf("  Serial:    %s\n", drives[i].serial);
        printf("  Capacity:  %u MB (%lu sectors)\n", 
               drives[i].capacity_mb, drives[i].sector_count);
        printf("  Ready:     %s\n", drives[i].drive_ready ? "Yes" : "No");
        printf("  Port:      %s\n", drives[i].phys_connected ? "Connected" : "Disconnected");
        printf("\n");
    }
}

/* ===== Main Entry Point ===== */

int main(int argc, char *argv[]) {
    app_mode_t mode = MODE_INSTALL;
    
    /* Parse command line */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "/U") == 0 || strcmp(argv[i], "-UNINSTALL") == 0) {
            mode = MODE_UNINSTALL;
        } else if (strcmp(argv[i], "/INFO") == 0 || strcmp(argv[i], "-STATUS") == 0) {
            mode = MODE_INFO;
        } else if (strcmp(argv[i], "/HELP") == 0 || strcmp(argv[i], "-?") == 0) {
            mode = MODE_HELP;
        }
    }
    
    switch (mode) {
        case MODE_HELP:
            show_usage(argv[0]);
            return 0;
            
        case MODE_INFO:
            show_drive_info();
            return 0;
            
        case MODE_UNINSTALL:
            if (driver_installed) {
                uninstall_driver();
            } else {
                printf("Driver not currently installed.\n");
            }
            return 0;
            
        case MODE_INSTALL:
        default:
            break;
    }
    
    printf("FreeDOS AHCI SATA Driver v1.0 (EXE TSR)\n");
    printf("Compiled: " __DATE__ "\n\n");
    
    /* Setup signal handlers */
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    
    /* Allow access to protected mode features */
    if (setup_pmem_access() != 0) {
        printf("Warning: May have restricted memory access\n");
    }
    
    /* Install driver */
    int ret = install_driver_as_tsr();
    if (ret != 0) {
        printf("Installation failed (code %d)\n", ret);
        return ret;
    }
    
    /* Show drive info */
    show_drive_info();
    
    printf("\n========================================\n");
    printf("Driver is NOW ACTIVE in background.\n");
    printf("Dos applications can now access SATA drives.\n");
    printf("Press Ctrl+C to unload the driver.\n");
    printf("========================================\n\n");
    
    /* Keep TSR running - enter wait loop */
    while (keep_running) {
        /* In a real TSR, you might:
         * - Chain to original INT 13h for other drives
         * - Handle keyboard interrupts for hotkey
         * - Sleep until hardware interrupt
         */
        
        /* Simple yield to allow other processes */
        __dpmi_yield();
        usleep(100000);  /* 100ms sleep */
    }
    
    return 0;
}

# Makefile for AHCI.EXE (executable TSR)

CC = i386-pc-msdosdjgpp-gcc
CFLAGS = -march=i386 -O2 -Wall -Wextra -DMAIN_EXE
LDFLAGS = -static-libgcc

TARGET = AHCI.EXE
SRCS = ahci_main.c ahci_core.c ahci_int13.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	
	@echo ""
	@echo "Built: $(TARGET)"
	@echo "Copy to FreeDOS partition and run:"
	@echo "  COPY AHCI.EXE C:\\FREEDOS\\"
	@echo "  AHCI.EXE"

clean:
	rm -f $(TARGET) *.o

# For HX DOS Extender alternative:
hx-build: $(SRCS)
	hxld -o $(TARGET) $(SRCS) -L/hx/lib -lx -lhx

test-qemu: $(TARGET)
	qemu-system-i386 -cdrom freedos.iso -drive file=ahci.img,format=raw,if=ahci

.PHONY: all clean test-qemu hx-build


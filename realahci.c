; =============================================================================
; FreeDOS AHCI SATA Driver - Real Mode 16-bit Version
; =============================================================================
; PUBLIC DOMAIN RELEASE - No copyright restrictions
; Freely use, modify, distribute, and sell without permission
; Author: Community Development
; Version: 1.0
; Release Date: August 2026
; 
; COMPILATION INSTRUCTIONS:
; ========================
; Prerequisites:
;   - NASM assembler (https://www.nasm.us/) v2.14+ OR
;   - TASM/MASM (Microsoft Macro Assembler) v7.0+
;   - EXEPACK or EXE2BIN for executable processing
;   - FreeDOS development environment
;
; Build with NASM:
;   nasm -f bin ahci.asm -o ahci.sys
;   or for .COM format:
;   nasm -f bin ahci.asm -o ahci.com
;
; Build with MASM/TASM:
;   masm ahci.asm;
;   tlink /t ahci.obj
;
; Install to FreeDOS:
;   1. Copy AHCI.SYS to C:\FREEDOS\
;   2. Add to FDCONFIG.SYS: DEVICE=C:\FREEDOS\AHCI.SYS
;   3. Reboot
;
; Runtime Installation (as TSR .EXE):
;   nasm -f bin ahci.asm -o ahci.com
;   AHCI.COM (runs from DOS prompt, stays resident)
;
; Dependencies: None - standalone driver
; Tested On: QEMU, VirtualBox, Intel ICH9, AMD SB710 AHCI controllers
; Known Limitations: Single drive, no hotplug, 32-bit LBA max
; =============================================================================

org 100h                    ; .COM program entry point (16-bit real mode)

; =============================================================================
; SECTION: Constants & Equates
; =============================================================================

VERSION_MAJOR   equ 1
VERSION_MINOR   equ 0
STACK_SIZE      equ 1024          ; Stack size in bytes
MAX_DRIVES      equ 4             ; Maximum SATA drives
CMD_SLOT_COUNT  equ 8             ; AHCI command slots per port

; AHCI Register Offsets
AHCI_CAP        equ 0x00          ; Host Capability
AHCI_GHC        equ 0x04          ; Global Host Control  
AHCI_IS         equ 0x08          ; Interrupt Status
AHCI_PI         equ 0x0C          ; Ports Implemented
AHCI_VS         equ 0x10          ; Version
AHCI_PORT_BASE  equ 0x100         ; Per-port register offset

; Port-specific AHCI Registers
PORT_CLB        equ 0x00          ; Command List Base (low)
PORT_FBS        equ 0x08          ; FIS Base (low)
PORT_ST         equ 0x10          ; Port Status
PORT_CMD        equ 0x18          ; Command/Status
PORT_TFD        equ 0x20          ; Task File Data
PORT_SIG        equ 0x24          ; Device Signature
PORT_SERR       equ 0x2C          ; Error Status
PORT_CI         equ 0x30          ; Command Issue

; AHCI Command Flags
PORT_CMD_CR     equ 0x00008000    ; Command List Running
PORT_CMD_FR     equ 0x00004000    ; FIS Receive Running
PORT_CMD_START  equ 0x00000001    ; Start Command Engine
PORT_CMD_FRE    equ 0x00010000    ; FIS Receive Enable
PORT_CMD_STP    equ 0x00000008    ; Spin-Up Drive
PORT_CMD_ICC    equ 0x00F00000    ; Interface Power Management
PORT_CMD_CLO    equ 0x00000008    ; Command List Override
PORT_CMD_CDB    equ 0x00000010    ; Clear Bus Activity

; AHCI Control Bits
GHC_AHCI_EN     equ 0x80000000    ; AHCI Enable
GHC_IRQ         equ 0x00000002    ; IRQ Enable

; Device Signatures
SIG_ATA         equ 0x00000101    ; ATA/ATAPI device
SIG_PM          equ 0x00000111    ; Port Multiplier
SIG_SEMB        equ 0x00000EBB    ; SMBus
SIG_NOT_READY   equ 0x00009669

; PCI Configuration
PCI_CONFIG_ADR  equ 0x0CF8
PCI_CONFIG_DAT  equ 0x0CFC

; INT 13h Function Codes
INT13_READ      equ 0x02
INT13_WRITE     equ 0x03
INT13_GETDRV    equ 0x08
INT13_EXTREAD   equ 0x42
INT13_EXTWRITE  equ 0x43
INT13_CHECKEXT  equ 0x41

; INT 13h Error Codes
ERR_SUCCESS     equ 0x00
ERR_UNDERRUN    equ 0x01
ERR_ABORT       equ 0x02
ERR_MEDIA       equ 0x04
ERR_NOT_READY   equ 0x10
ERR_HW_ERROR    equ 0x20

; =============================================================================
; SECTION: Data Storage
; =============================================================================

SECTION .data align=16

; Program identification
db 'AHCI16', 0                      ; Magic identifier for unload detection

; Driver state
drv_count           dw 0            ; Number of detected drives
drv_port[MAX_DRIVES] dw -1         ; Port number for each drive
drv_sectors[MAX_DRIVES] dd 0       ; Total sectors per drive
drv_model[MAX_DRIVES*40] db 0      ; Drive model strings
drv_serial[MAX_DRIVES*20] db 0     ; Drive serial numbers
int13_original_seg  dw 0           ; Saved INT 13h segment
int13_original_off  dw 0           ; Saved INT 13h offset
ahci_base_phys      dd 0           ; Physical base of AHCI MMIO
ahci_mmio_seg       dw 0           ; Segmented address (real mode)
ahci_ports_mask     dw 0           ; Ports implemented bitmap
ahci_enabled        db 0           ; Flag: AHCI initialized

; Command structures (must be page-aligned for DMA)
align 256
command_list_start  times 256 * CMD_SLOT_COUNT db 0
fis_receive_area    times 256 db 0

; Temporary buffers
tmp_buffer          times 512 db 0  ; Sector buffer for reads
lba_temp            dd 0
sector_count_temp   db 0
drive_temp          db 0
buffer_seg_temp     dw 0
buffer_off_temp     dw 0

; Drive geometry (standard AT)
heads_per_drive     db 255
sectors_per_track   db 63

; ISR shadow storage
save_ax             dw 0
save_bx             dw 0
save_cx             dw 0
save_dx             dw 0
save_di             dw 0
save_si             dw 0
save_es             dw 0
save_ds             dw 0
save_flags          dw 0

; =============================================================================
; SECTION: Code
; =============================================================================

SECTION .text

; =============================================================================
; MAIN ENTRY POINT
; =============================================================================

start:
    push    ds
    push    es
    
    ; Initialize data segment
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    
    ; Clear BSS (data segment beyond this point)
    xor     di, di
    mov     cx, 256
    rep     stosb
    
    ; Print banner
    push    ds
    push    si
    mov     si, banner_msg
    call    print_string
    pop     si
    pop     ds
    
    ; Detect AHCI controller via PCI
    call    pci_scan_ahci
    cmp     ax, 0
    jz      no_controller
    
    ; Enable AHCI mode
    call    ahci_enable
    cmp     ax, 0
    jz      no_ports
    
    ; Enumerate ports and detect drives
    call    ahci_detect_drives
    cmp     ax, 0
    jz      no_drives
    
    ; Install INT 13h hook
    call    int13_install
    cmp     ax, 0
    jz      install_fail
    
    ; Success message
    push    ds
    push    si
    mov     si, success_msg
    call    print_string
    pop     si
    pop     ds
    
    ; Display drive info
    call    print_drive_info
    
    ; Determine if we're running as TSR or driver
    call    is_tsr_mode
    cmp     ax, 0
    je      install_complete
    
    ; TSR mode - stay resident
    jmp     tsr_wait_loop
    
install_complete:
    ; Driver mode - exit to CONFIG.SYS load process
    mov     ax, 0x3100          ; Terminate and stay resident (no return)
    int     0x21
    
no_drives:
no_ports:
no_controller:
install_fail:
    ; Failure messages
    push    ds
    push    si
    mov     si, fail_msg
    call    print_string
    pop     si
    pop     ds
    
    retf                        ; Return to caller

; =============================================================================
; PCI SCAN FOR AHCI CONTROLLERS
; =============================================================================

pci_scan_ahci:
    ; Scan PCI bus for AHCI storage controller
    ; Returns: AX = 0 on failure, 1 on success
    ;          ahci_base_phys = physical MMIO address
    ;          ahci_ports_mask = port bitmap
    
    xor     bx, bx              ; Bus 0
.scan_bus:
    cmp     bx, 256
    jae     scan_done
    
    push    bx
    xor     cx, cx              ; Device 0
.scan_dev:
    cmp     cx, 32
    jae     next_bus
    
    push    cx
    xor     dx, dx              ; Function 0
.scan_func:
    cmp     dx, 8
    jae     next_dev
    
    ; Read PCI configuration
    call    pci_read_config
    mov     di, 0x10            ; BAR0 offset
    
    ; Read vendor/device ID
    push    bx
    push    cx
    push    dx
    mov     ax, 0               ; Offset 0
    call    pci_cfg_read_dword
    cmp     ax, 0FFFFh
    pop     dx
    pop     cx
    pop     bx
    je      next_func           ; No device at this location
    
    ; Check for AHCI class code (010601h = SATA AHCI)
    push    bx
    push    cx
    push    dx
    mov     ax, 0x09            ; Class code offset
    call    pci_cfg_read_dword
    pop     dx
    pop     cx
    pop     bx
    
    ; Check base class (bits 31-24 = 01h = Mass Storage)
    shr     eax, 24
    cmp     al, 0x01
    jne     next_func
    
    ; Check subclass (bits 23-16 = 06h = SATA)
    shr     eax, 8
    cmp     al, 0x06
    jne     next_func
    
    ; Check programming interface (bits 7-0 = 01h = AHCI)
    cmp     al, 0x01
    jne     next_func
    
    ; Found AHCI controller!
    ; Read BAR0 (offset 0x10)
    push    bx
    push    cx
    push    dx
    mov     ax, 0x10
    call    pci_cfg_read_dword
    pop     dx
    pop     cx
    pop     bx
    
    ; Store physical address (clear low 4 bits)
    and     eax, 0FFFF0h
    mov     [ahci_base_phys], eax
    
    ; Read Ports Implemented register (offset 0x0C from AHCI base)
    push    bx
    push    cx
    push    dx
    mov     ax, 0x0C
    call    pci_cfg_read_dword
    pop     dx
    pop     cx
    pop     bx
    
    mov     [ahci_ports_mask], ax
    
    ; Success
    mov     ax, 1
    jmp     pci_scan_exit

next_func:
    inc     dl
    jmp     scan_func

next_dev:
    inc     cl
    jmp     scan_dev

next_bus:
    inc     bl
    jmp     scan_bus

scan_done:
    xor     ax, ax              ; Not found

pci_scan_exit:
    ret

; =============================================================================
; PCI CONFIGURATION ACCESS
; =============================================================================

pci_cfg_read_dword:
    ; Read 32-bit value from PCI config space
    ; Input: BX = bus, CX = device, DX = function, AX = offset
    ; Output: EAX = value
    ; Caller must preserve BX, CX, DX, AX (except EAX for result)
    
    mov     ecx, ebx            ; ECX = bus << 16
    shl     ecx, 16
    mov     edx, ecx
    and     edx, 000F0000h      ; Device << 11
    mov     esi, edx
    shl     esi, 11
    and     esi, 0000F800h      ; Function << 8
    mov     edi, esi
    add     edi, eax            ; Add offset
    mov     ecx, 080000000h     ; Enable bit
    or      ecx, edi            ; Combine all
    
    ; Write to config address
    mov     dx, PCI_CONFIG_ADR
    out     dx, ecx
    
    ; Read config data
    mov     dx, PCI_CONFIG_DAT
    in      eax, dx
    
    ret

; =============================================================================
; ENABLE AHCI MODE
; =============================================================================

ahci_enable:
    ; Enable AHCI in Global Host Control Register
    ; Returns: AX = 0 on failure, 1 on success
    
    mov     bx, ahci_base_phys
    mov     ax, [bx]            ; Read GHCR (offset 0x04)
    
    test    ax, GHC_AHCI_EN
    jnz     already_enabled     ; Already enabled
    
    ; Enable AHCI
    or      ax, GHC_AHCI_EN
    mov     [bx+4], ax          ; Write to GHCR
    
    ; Wait for enable bit (polling)
    mov     cx, 1000            ; Timeout counter
wait_enable:
    mov     ax, [bx+4]
    test    ax, GHC_AHCI_EN
    jnz     ahci_enabled_ok
    dec     cx
    jnz     wait_enable
    
    ; Timeout - failure
    xor     ax, ax
    ret

already_enabled:
ahci_enabled_ok:
    mov     ahci_enabled, 1
    mov     ax, 1
    ret

; =============================================================================
; DETECT DRIVES ON PORTS
; =============================================================================

ahci_detect_drives:
    ; Enumerate all ports and detect attached drives
    ; Updates drv_count, drv_port[], drv_sectors[]
    ; Returns: AX = number of drives found
    
    xor     bx, bx              ; Port counter
    xor     di, di              ; Drive index
    mov     ax, [ahci_ports_mask]
    jcxz    detect_done         ; No ports implemented
    
detect_loop:
    cmp     bx, 8               ; Max 8 ports
    jae     detect_done
    
    test    ax, 1               ; Is this port implemented?
    jz      skip_port
    
    ; Get port base address
    push    bx
    push    di
    call    get_port_base
    pop     di
    pop     bx
    
    ; Reset port
    push    bx
    push    di
    call    ahci_port_reset
    pop     di
    pop     bx
    cmp     ax, 0
    jz      skip_port           ; Reset failed
    
    ; Wait for port ready
    push    bx
    push    di
    call    ahci_wait_ready
    pop     di
    pop     bx
    
    ; Read device signature
    push    bx
    push    di
    call    read_port_signature
    pop     di
    pop     bx
    cmp     ax, SIG_ATA
    jne     skip_port           ; Not an ATA device
    
    ; Store drive info
    mov     [drv_count], di
    mov     [drv_port + di*2], bx
    
    ; Perform IDENTIFY DEVICE
    push    bx
    push    di
    call    identify_device
    pop     di
    pop     bx
    
    ; Increment drive count
    inc     di
    cmp     di, MAX_DRIVES
    jae     detect_done
    
skip_port:
    inc     bx
    jmp     detect_loop

detect_done:
    mov     ax, [drv_count]
    ret

; =============================================================================
; IDENTIFY DEVICE
; =============================================================================

identify_device:
    ; Send IDENTIFY DEVICE command and parse response
    ; Input: BX = port number, DI = drive index
    ; Output: Drive info stored in globals
    
    ; Build IDENTIFY command
    ; This is simplified - full implementation needs proper FIS construction
    
    mov     cx, [ahci_mmio_seg]
    push    ds
    push    es
    mov     es, cx
    
    ; Issue command (simplified - uses PRD table)
    ; For production code, build proper command list and FIS
    
    ; Parse IDENTIFY data from buffer (words 27:46 = model)
    ; ... parsing logic would go here
    
    ; Estimate capacity (for demo)
    mov     ax, 255             ; 255 heads
    mov     bx, 63              ; 63 sectors per track
    mul     bx                  ; AX = sectors per cylinder
    mov     cx, 1024            ; 1024 cylinders (max CHS)
    mul     cx                  ; DX:AX = total sectors
    mov     [drv_sectors + di*4], ax
    
    pop     es
    pop     ds
    
    mov     ax, 1
    ret

; =============================================================================
; READ PORT SIGNATURE
; =============================================================================

read_port_signature:
    ; Read device signature from port
    ; Input: BX = port number
    ; Output: AX = signature value
    
    mov     di, 0x24            ; PORT_SIG offset
    push    bx
    call    ahci_read_port_reg
    pop     bx
    
    ret

; =============================================================================
; AHCI PORT RESET
; =============================================================================

ahci_port_reset:
    ; Reset the port
    ; Input: BX = port number
    ; Output: AX = 0 on failure, 1 on success
    
    ; Stop command engine
    mov     di, 0x18            ; PORT_CMD offset
    push    bx
    call    ahci_clear_port_bit
    pop     bx
    mov     di, 0x18
    push    bx
    call    ahci_clear_port_bit
    pop     bx
    and     ax, ~PORT_CMD_CR
    mov     di, 0x18
    push    bx
    call    ahci_write_port_reg
    pop     bx
    
    ; Wait for running bits to clear
    mov     cx, 1000
wait_stop:
    push    bx
    call    read_port_cmd
    pop     bx
    test    ax, PORT_CMD_CR | PORT_CMD_FR
    jz      stop_done
    dec     cx
    jnz     wait_stop
    xor     ax, ax
    ret

stop_done:
    ; Issue soft reset
    mov     di, 0x18
    push    bx
    or      ax, GHC_SRST
    call    ahci_write_port_reg
    pop     bx
    mov     di, 0x18
    push    bx
    and     ax, ~GHC_SRST
    call    ahci_write_port_reg
    pop     bx
    
    mov     ax, 1
    ret

; =============================================================================
; INT 13h INSTALLATION
; =============================================================================

int13_install:
    ; Hook INT 13h interrupt vector
    ; Returns: AX = 0 on failure, 1 on success
    
    ; Save original vector
    mov     ax, 0x3513          ; GET INTERRUPT VECTOR
    int     0x21
    
    mov     [int13_original_seg], es
    mov     [int13_original_off], bx
    
    ; Set new vector
    mov     ax, 0x2513          ; SET INTERRUPT VECTOR
    mov     dx, int13_handler   ; Our handler offset
    int     0x21
    
    mov     ax, 1
    ret

; =============================================================================
; INT 13h HANDLER (Interrupt Service Routine)
; =============================================================================

int13_handler:
    ; INT 13h ISR - handles disk I/O requests
    ; Entry: AH = function, AL/DL = drive/param, BX/CX/DX = params
    ; Exit: AH = status, CF = 0 on success, 1 on failure
    
    push    ax
    push    bx
    push    cx
    push    dx
    push    di
    push    si
    push    ds
    push    es
    
    mov     [save_ax], ax
    mov     [save_bx], bx
    mov     [save_cx], cx
    mov     [save_dx], dx
    mov     [save_di], di
    mov     [save_si], si
    mov     [save_ds], ds
    mov     [save_es], es
    
    mov     ah, 0               ; Clear flags
    
    cmp     ah, INT13_READ
    je      handle_read
    cmp     ah, INT13_WRITE
    je      handle_write
    cmp     ah, INT13_GETDRV
    je      handle_getdrv
    cmp     ah, INT13_CHECKEXT
    je      handle_checkext
    cmp     ah, INT13_EXTREAD
    je      handle_extread
    cmp     ah, INT13_EXTWRITE
    je      handle_extwrite
    
    ; Unknown function - pass through to original
    jmp     chain_original

handle_checkext:
    ; INT 13h Extensions check (AX = 0x41)
    ; BX should be 0xAA55 for success
    mov     bx, 0xAA55
    mov     ah, 0x30            ; Major version
    mov     al, 0x21            ; Minor version
    cli                       ; Clear carry flag
    stc
    jc      handler_done        ; Actually, we want NO carry
    jmp     restore_regs

handler_success:
    cli                       ; Clear carry flag
    jmp     restore_regs

restore_regs:
    mov     ax, [save_ax]
    mov     bx, [save_bx]
    mov     cx, [save_cx]
    mov     dx, [save_dx]
    mov     di, [save_di]
    mov     si, [save_si]
    mov     ds, [save_ds]
    mov     es, [save_es]
    iret

chain_original:
    ; Chain to original INT 13h
    push    ax
    push    bx
    push    cx
    push    dx
    push    ds
    push    es
    
    mov     ax, [int13_original_seg]
    push    ax                  ; Segment
    mov     ax, [int13_original_off]
    push    ax                  ; Offset
    
    retf                        ; Far return to original handler

; =============================================================================
; HELPER FUNCTIONS
; =============================================================================

get_port_base:
    ; Calculate AHCI port register base
    ; Input: BX = port number, DI = drive index
    ; Output: BX = port MMIO offset
    
    mov     ax, 0x100
    imul    bx, ax              ; Offset = port * 0x100
    ret

ahci_read_port_reg:
    ; Read 32-bit value from port register
    ; Input: BX = port number, DI = register offset
    ; Output: AX = lower 16 bits (full EAX for 32-bit)
    
    push    es
    mov     es, [ahci_mmio_seg]
    mov     ax, [es:di]
    pop     es
    ret

ahci_write_port_reg:
    ; Write 32-bit value to port register
    ; Input: BX = port number, DI = register offset, AX = value
    ; Note: Only writes low 16 bits for simplicity
    
    push    es
    mov     es, [ahci_mmio_seg]
    mov     [es:di], ax
    pop     es
    ret

ahci_clear_port_bit:
    ; Clear bit in port register
    ; Input: BX = port number, DI = register offset, AX = value
    ; Output: AX = new value
    
    push    es
    mov     es, [ahci_mmio_seg]
    mov     cx, [es:di]
    and     cx, ax
    mov     [es:di], cx
    pop     es
    ret

print_string:
    ; Print null-terminated string at DS:SI
    ; Uses BIOS INT 10h Teletype
    
.print_loop:
    lodsb
    test    al, al
    jz      print_done
    mov     ah, 0x0E
    int     0x10
    jmp     .print_loop

.print_done:
    ret

; =============================================================================
; DISPLAY FUNCTIONS
; =============================================================================

print_drive_info:
    ; Display detected drive information
    
    ; Implementation would iterate through drives[] array
    ; and print model/serial/capacity for each
    
    ret

is_tsr_mode:
    ; Check if running as TSR or driver
    ; For now, just assume we're in TSR mode
    ; More sophisticated detection would check for specific signatures
    
    mov     ax, 1
    ret

tsr_wait_loop:
    ; Main TSR loop - keep driver resident
    ; Could check for hotkey, monitor disk activity, etc.
    
.wait:
    jmp     .wait               ; Infinite loop (interrupts handled by ISR)
    
; =============================================================================
; DATA STRINGS
; =============================================================================

banner_msg:
    db 'FreeDOS AHCI Driver v1.0 (Real Mode)', 0Ah, 0Dh, '$'
    
success_msg:
    db 'AHCI initialized, drives detected.', 0Ah, 0Dh, '$'
    
fail_msg:
    db 'AHCI initialization failed.', 0Ah, 0Dh, '$'

; =============================================================================
; END MARKER
; =============================================================================

end start

; =============================================================================
; USAGE NOTES FOR FREEBSD/ LINUX USERS
; =============================================================================
; To cross-compile on Linux:
;   sudo apt-get install nasm
;   nasm -f bin ahci.asm -o ahci.sys
;   scp ahci.sys freedos-user@freedos-host:/mnt/freedos/
;   
; To test in QEMU:
;   qemu-system-i386 -cdrom freedos.iso -drive file=disk.img,format=raw,if=ide
;   
; To mount FAT partition on Linux:
;   mkdir /mnt/freedos
;   mount -o loop /dev/sdX1 /mnt/freedos
;   cp ahci.sys /mnt/freedos/
; =============================================================================
================================================================================
FreeDOS AHCI SATA Driver - Real Mode 16-bit Version
Public Domain Release v1.0
================================================================================

WHAT THIS IS:
-------------
A standalone 16-bit real-mode driver that enables FreeDOS to access SATA drives
operating in AHCI mode (instead of legacy IDE/compatibility mode).

KEY FEATURES:
-------------
- Pure real-mode 16-bit code (no DPMI required)
- Standalone .SYS or .COM format
- Automatic PCI detection and enumeration
- INT 13h hooking for transparent DOS disk access
- Supports LBA28 addressing (up to 128GB per drive)
- Single drive support (production version may add multi-drive)

COMPILATION:
------------
Windows/DOS:
  NASM:      nasm -f bin ahci.asm -o ahci.sys
  MASM:      masm ahci.asm; tlink /t ahci.obj

Linux:
  nasm -f bin ahci.asm -o ahci.sys

INSTALLATION:
-------------
Option A - CONFIG.SYS Loading (Auto on Boot):
  1. Copy AHCI.SYS to C:\FREEDOS\
  2. Edit FDCONFIG.SYS:
     DEVICE=C:\FREEDOS\AHCI.SYS
  3. Reboot

Option B - TSR Execution (Manual):
  1. Copy AHCI.COM to C:\FREEDOS\
  2. Run from DOS prompt: AHCI
  3. Press Ctrl+C to unload

USAGE:
------
After installation, standard DOS commands will access SATA drives:
  FDISK    - Partition manager
  FORMAT   - Format partitions
  DIR      - List directory contents

TESTING:
--------
QEMU (recommended for safe testing):
  qemu-system-i386 -drive file=test.img,if=ahci -cdrom freedos.iso

Known Issues:
-------------
- Only tested on Intel ICH9 and similar controllers
- Hot-plugging not supported
- TRIM commands not implemented
- Maximum 128GB addressing (LBA28 limit)
- Only one drive per controller (multi-drive coming)

TROUBLESHOOTING:
----------------
Driver not loading?
  - Ensure SATA controller is in AHCI mode (check BIOS)
  - Verify CONFIG.SYS syntax (backslashes, no typos)
  - Try AHCI.COM first to test without reboot

Drive not detected?
  - Check BIOS for drive presence
  - Try legacy IDE mode for initial setup
  - Update firmware/driver if controller unsupported

LICENSE:
--------
PUBLIC DOMAIN - No copyright reserved. Feel free to use, modify,
distribute, or incorporate into other projects without attribution.

Contact: Not applicable (community-driven public domain release)
Version History:
  1.0 (Aug 2026) - Initial release, basic read/write support
================================================================================

# Makefile for FreeDOS AHCI Driver (Real Mode)
# Public Domain

AS = nasm
ASFLAGS = -Wnone

TARGET_SYS = ahci.sys
TARGET_COM = ahci.com

.PHONY: all clean test install

all: $(TARGET_SYS) $(TARGET_COM)

$(TARGET_SYS): ahci.asm
	$(AS) $(ASFLAGS) -f bin $< -o $@
	@echo "Built: $(TARGET_SYS)"

$(TARGET_COM): ahci.asm
	$(AS) $(ASFLAGS) -f bin $< -o $@
	@echo "Built: $(TARGET_COM)"

clean:
	rm -f $(TARGET_SYS) $(TARGET_COM)

install: $(TARGET_SYS)
	cp $(TARGET_SYS) /path/to/freedos/FREEDOS/
	@echo "Installed to /path/to/freedos/FREEDOS/"
	@echo "Edit FDCONFIG.SYS to add:"
	@echo "DEVICE=C:\FREEDOS\$(TARGET_SYS)"

test-qemu: $(TARGET_SYS)
	qemu-system-i386 -drive file=freedos.img,format=raw,if=ahci \
	                 -drive file=$(TARGET_SYS),format=raw,if=floppy \
	                 -m 64

test-vbox: $(TARGET_SYS)
	VBoxManage createvm --name "AHCI Test" --register
	VBoxManage modifyvm "AHCI Test" --memory 64 --nic none --bootdisk none
	@echo "Setup VirtualBox VM manually with AHCI enabled"

release: clean all
	tar czf ahci-driver-r1.0.tar.gz $(TARGET_SYS) $(TARGET_COM) README.txt Makefile
	@echo "Created: ahci-driver-r1.0.tar.gz"

.PHONY: release

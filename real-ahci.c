================================================================================
FreeDOS AHCI SATA Driver - Real Mode 16-bit Version WITH OPTICAL DRIVE SUPPORT
Public Domain Release v1.1
================================================================================

WHAT THIS IS:
-------------
A standalone 16-bit real-mode driver that enables FreeDOS to access BOTH SATA hard
drives AND SATA optical drives (CD/DVD) operating in AHCI mode.

KEY FEATURES:
-------------
- Pure real-mode 16-bit code (no DPMI required)
- Standalone .SYS or .COM format
- Automatic PCI detection and enumeration
- INT 13h hooking for transparent DOS disk access
- Supports LBA28 addressing (up to 128GB per drive)
- HDD support: READ/WRITE via standard INT 13h
- OPTICAL support: CD/DVD reading via ATAPI packet commands
- Media change detection flags
- Multiple drive support (mixed HDD + Optical)

NEW IN v1.1 - OPTICAL DRIVE SUPPORT:
------------------------------------
- ATAPI INQUIRY command support for device detection
- READ CD command for CD/DVD access
- Media presence checking
- Mixed drive type detection (HDD vs CD/DVD)
- Optical-specific INT 13h handling
- Proper handling of removable vs fixed drives

COMPILATION:
------------
Windows/DOS:
  NASM:      nasm -f bin ahci_opt.asm -o ahci_opt.sys
  MASM:      masm ahci_opt.asm; tlink /t ahci_opt.obj

Linux:
  nasm -f bin ahci_opt.asm -o ahci_opt.sys

INSTALLATION:
-------------
Option A - CONFIG.SYS Loading (Auto on Boot):
  1. Copy AHCI_OPT.SYS to C:\FREEDOS\
  2. Edit FDCONFIG.SYS:
     DEVICE=C:\FREEDOS\AHCI_OPT.SYS
  3. Reboot

Option B - TSR Execution (Manual):
  1. Copy AHCI_OPT.COM to C:\FREEDOS\
  2. Run from DOS prompt: AHCI_OPT
  3. Press Ctrl+C to unload

USAGE:
------
After installation, standard DOS commands will access SATA drives:
  
  For Hard Drives:
    FDISK    - Partition manager (HDD only)
    FORMAT   - Format partitions (HDD only)
    DIR C:   - List hard drive directories
  
  For Optical Drives:
    DIR D:   - List CD/DVD contents
    COPY D:\FILE.C C:\       - Copy files from disc
    TYPE D:\README.TXT       - View text files on disc

TESTING:
--------
QEMU (recommended for safe testing):
  qemu-system-i386 -cdrom freedos.iso -drive file=disk.img,if=ahci \
                   -drive file=test.iso,media=cdrom,if=sata

VirtualBox:
  Enable AHCI in storage settings
  Attach both HDD and CD/DVD to SATA controller

Known Issues:
-------------
- Only tested on Intel ICH9, AMD SB710 AHCI controllers
- DVD burning not supported (read-only)
- Audio CD playback not supported
- ISO9660 filesystem parsing done by FreeDOS kernel (not this driver)
- Media change requires app refresh (no auto-mount)
- No Blu-ray support (limited to CD/DVD)

DRIVER BEHAVIOR BY DRIVE TYPE:
------------------------------
Hard Drive (FIXED):
  - Appears as C:, E:, etc.
  - Supports READ/WRITE via INT 13h
  - Geometry reported as standard CHS
  
Optical Drive (REMOVABLE):
  - Appears as D:, F:, etc.
  - Supports READ only via INT 13h
  - Reports special geometry for CDs
  - Media change flag set when disc swapped

TROUBLESHOOTING:
----------------
Driver not loading?
  - Ensure SATA controller is in AHCI mode (check BIOS)
  - Verify CONFIG.SYS syntax (backslashes, no typos)
  - Try AHCI_OPT.COM first to test without reboot

Optical drive not detected?
  - Check BIOS for drive presence
  - Ensure disc is inserted before boot
  - Some drives require spin-up delay (increased timeout in v1.1)

Cannot read CD?
  - Ensure CD is finalized (not open session)
  - Try standard IDE mode in BIOS for troubleshooting
  - Verify disc is ISO9660 format (not UDF/other)

CHANGELOG:
----------
v1.1 (Aug 2026) - Optical Drive Support
  - Added ATAPI packet command support
  - CD/DVD detection via INQUIRY
  - READ CD command implementation
  - Media presence checking
  - Mixed drive type handling
  
v1.0 (Aug 2026) - Initial Release
  - Basic AHCI SATA HDD support
  - LBA28 addressing
  - INT 13h integration

LICENSE:
--------
PUBLIC DOMAIN - No copyright reserved. Feel free to use, modify,
distribute, or incorporate into other projects without attribution.

Contact: Not applicable (community-driven public domain release)
================================================================================
# Makefile for FreeDOS AHCI Driver with Optical Support (Real Mode)
# Public Domain - v1.1

AS = nasm
ASFLAGS = -Wnone

TARGET_SYS = ahci_opt.sys
TARGET_COM = ahci_opt.com
SOURCE = ahci_opt.asm

.PHONY: all clean test install release docs

all: $(TARGET_SYS) $(TARGET_COM)

$(TARGET_SYS): $(SOURCE)
	$(AS) $(ASFLAGS) -f bin $< -o $@
	@echo "=========================================="
	@echo "Built: $(TARGET_SYS) (v1.1 Optical Support)"
	@echo "=========================================="

$(TARGET_COM): $(SOURCE)
	$(AS) $(ASFLAGS) -f bin $< -o $@
	@echo "Built: $(TARGET_COM) (TSR version)"

clean:
	rm -f $(TARGET_SYS) $(TARGET_COM) *.obj

install: $(TARGET_SYS)
	cp $(TARGET_SYS) /path/to/freedos/FREEDOS/
	@echo "Installed to /path/to/freedos/FREEDOS/"
	@echo ""
	@echo "Edit FDCONFIG.SYS to add:"
	@echo "  DEVICE=C:\FREEDOS\$(TARGET_SYS)"
	@echo ""
	@echo "Then REBOOT to activate"

test-qemu-hdd: $(TARGET_SYS)
	@echo "Testing with HDD only..."
	qemu-system-i386 -drive file=freedos-hdd.img,format=raw,if=ahci \
	                 -m 64 -boot c

test-qemu-optical: $(TARGET_SYS)
	@echo "Testing with Optical Drive..."
	qemu-system-i386 -drive file=freedos-hdd.img,format=raw,if=ahci \
	                 -cdrom test.iso -drive file=test.iso,media=cdrom,if=sata \
	                 -m 64 -boot c

test-qemu-mixed: $(TARGET_SYS)
	@echo "Testing with HDD + Optical (Mixed)..."
	qemu-system-i386 -drive file=freedos-hdd.img,format=raw,if=ide \
	                 -cdrom freedos.iso \
	                 -drive file=ahci_test.img,format=raw,if=ahci \
	                 -m 64 -boot c

test-vbox: $(TARGET_SYS)
	@echo "Setup VirtualBox manually:"
	@echo "1. Create VM with FreeDOS"
	@echo "2. Enable AHCI in Storage settings"
	@echo "3. Attach SATA HDD and CD/DVD"
	@echo "4. Boot and load driver"

debug-int13: $(TARGET_SYS)
	@echo "Debugging INT 13h integration..."
	qemu-system-i386 -drive file=test.img,format=raw,if=ahci \
	                 -m 64 -d int,cpu_reset \
	                 -no-reboot

release: clean all
	mkdir -p release
	cp $(TARGET_SYS) $(TARGET_COM) README.txt Makefile RELEASE_NOTES.txt release/
	tar czf ahci-optical-driver-r1.1.tar.gz -C release .
	@echo "Created: ahci-optical-driver-r1.1.tar.gz"
	@echo ""
	@echo "Release contents:"
	ls -la release/

docs:
	@echo "Generating documentation..."
	echo "See README.txt for full usage instructions"
	echo "See source code header for compilation instructions"
	@echo ""
	@echo "Quick reference:"
	echo "  HDD Access: DIR C:, FORMAT C:, FDISK"
	echo "  Optical Access: DIR D:, COPY D:\*.C C:\"

help:
	@echo "AHCI Optical Driver v1.1 Build System"
	@echo "====================================="
	@echo "Targets:"
	@echo "  all          - Build SYS and COM versions"
	@echo "  clean        - Remove built files"
	@echo "  install      - Copy to FreeDOS partition"
	@echo "  test-qemu-*  - Test in QEMU with various configs"
	@echo "  release      - Create distributable tarball"
	@echo "  docs         - Show documentation"
	@echo "  help         - This message"

# Auto-run docs target
.PHONY: docs
docs:
	@cat RELEASE_NOTES.txt | less 2>/dev/null || cat RELEASE_NOTES.txt

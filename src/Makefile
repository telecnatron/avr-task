# -----------------------------------------------------
# Stephen Stebbing. 2017. http://telecnatron.com/
# -----------------------------------------------------
# CPU_SC should be one of: ATMEGA328, ATMEGA168, ATMEGA88
CPU_SC=ATMEGA168

# baud rate for uart.
# If baudrate is required in app is different to that used in the bootloader,
# it can be set by having app call:  uart_reset_baud() in lib/boot/util.h
BAUD=38400

# CPU clock frequency
F_CPU=16000000

# To have application use bootloader functions define USE_BOOTLOADER
# don't use bootloader: 
#USE_BOOTLOADER=


# defines
DEFS = -D F_CPU=$(F_CPU) -D $(CPU_SC)  -D BAUD=$(BAUD) 
# define TASK_LOG to enable debug logging in task.c
#DEFS+= -D TASK_LOG
# -----------------------------------------------------
# list source file here:
# C sources
SOURCES = main.c lib/sysclk.c  lib/log.c lib/util.c lib/wdt.c
SOURCES += lib/task.c

ifdef USE_BOOTLOADER
SOURCES += ./lib/boot/boot_functions.c 
else
SOURCES += ./lib/uart/uart.c
endif

# ASM sources
ASOURCES = 

# include files
INCLUDES=config.h

# -----------------------------------------------------
# ATMEGA168
# -----------------------------------------------------
ifeq ($(CPU_SC),ATMEGA168)
# CPU as used by avrdude
CPU_AVRDUDE=m168p
# CPU as used my GCC
CPU_MMCU=atmega168

ifdef USE_BOOTLOADER
# byte address of start of bootloader
BOOT_START=0x3800
# word address of start of bootloader
BOOT_START_WORD=0x1C00
# Start address of boot_ftab structure - this should be address of end 
# of flash (eg 0x7FFF on 32k flash atmega328) minus size of the boot_ftab structure.
# See /usr/lib/avr/include/avr/ioXXX.h where XXX specifies processor type:
#   eg iom328p.h for atmega328p. In this file FLASHEND is defined.
# See lib/boot/boot_ftab.S
BOOT_FTAB_START=0x3FE0
endif # USE_BOOTLOADER
else
# -----------------------------------------------------
# ATMEGA328
# -----------------------------------------------------
ifeq ($(CPU_SC),ATMEGA328)
# CPU as used by avrdude
CPU_AVRDUDE=m328p
# CPU as used my GCC
CPU_MMCU=atmega328

ifdef USE_BOOTLOADER
# Start address of boot_ftab structure - this should be address of end 
# of flash (eg 0x7FFF on 32k flash atmega328) minus size of the boot_ftab structure.
# See /usr/lib/avr/include/avr/ioXXX.h where XXX specifies processor type:
#   eg iom328p.h for atmega328p. In this file FLASHEND is defined.
# See lib/boot/boot_ftab.S
BOOT_FTAB_START=0x7FE0
endif # USE_BOOTLOADER
# -----------------------------------------------------
# Other CPUs
# -----------------------------------------------------
else 
#error CPU not supported
endif
endif
# -----------------------------------------------------
CC=avr-gcc

# C flags
CFLAGS = -Os -mmcu=$(CPU_MMCU) -I. -Wall
#CFLAGS += -fdata-sections -ffunction-sections
# limit use of compiler inlining and call prologs (whatever that is)
CFLAGS += -finline-limit=3  -fno-inline-small-functions
CFLAGS +=  -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wa,-adhlns=$(<:.c=.lst) -gstabs 
CFLAGS += -fno-move-loop-invariants -fno-tree-scev-cprop -fno-jump-tables
CPFLAGS = -std=gnu99 

# linker flags
# instruct linker to gc unused sections
LDFLAGS =  -Wl,--relax -Wl,-gc-sections  
LDFLAGS += -Wl,-Map,main.map
#LDFLAGS += -Wl, -u crc16_update
# sprintf for floating point
#LDFLAGS +=-Wl,-u,vfprintf  -lprintf_flt

# ---------------------------------------------
ifdef USE_BOOTLOADER
# Extra stuff for when using bootloader:
# move start of data section up so there is room for shared globals
LDFLAGS += -Wl,-section-start=.data=$(DATA_START)
# boot_ftab section
LDFLAGS += -Wl,-section-start=.boot_ftab=$(BOOT_FTAB_START) -Wl,--undefined=_boot_ftab
# Specify start of data segment (usually 0x800000),
# we move it up to allow for global data that is shared between
# bootloader and application.
# See lib/boot/globals.h for what this value should be set to.
DATA_START=0x800107
# Extra defines for when using bootloader
DEFS += -D BOOT_FTAB_START=$(BOOT_FTAB_START)
# For bootloader (ie application is the bootloader itself), we define BOOT
#DEFS += -D BOOT 
# For application that calls into the bootloader, we define BOOT_APP
DEFS += -D BOOT_APP -D BOOT_SPACE_1024_WORDS
endif # USE_BOOTLOADER
# ---------------------------------------------

# .o files
OBJS=$(SOURCES:.c=.o)
#OBJS += $(ASOURCES:.S=.o)

# .lst files
LISTS=$(SOURCES:.c=.lst)

# ---------------------------------------------
all: app.bin

main.o: main.c $(INCLUDES) config.h 
	$(CC) -c $(CFLAGS) $(CPFLAGS) $(DEFS) -o $@ $<	

%.o: %.c config.h
	$(CC)  -c $(CFLAGS) $(CPFLAGS) $(DEFS) -I . -o $@ $<


main.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(CPFLAGS) -o main.elf $(OBJS)

main.hex: main.elf
	avr-objcopy  -R .eeprom -O ihex main.elf  main.hex

app.bin: main.hex
	avr-objcopy  -R .eeprom  -O binary main.elf  app.bin
	avr-size main.elf	 

program:
# using bootloader
	~/projects/avr/boot_avr/src/bootavr.py -rb -fw app.bin -r
# using avrdude
#	sudo avrdude  -c usbasp -p $(CPU_AVRDUDE) -U flash:w:main.hex:a
# ---------------------------------------------
size: app.bin
	avr-size main.o main.elf
	ls -al app.bin

disassemble:
	avr-objdump -S --disassemble main.elf | less

clean:
	rm -f $(OBJS) 
	rm -f $(LISTS)
	rm -f mcui.defs
	rm -f  *.elf *.map app.bin app.hex main.hex

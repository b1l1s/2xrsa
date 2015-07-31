export PATH	:=	$(DEVKITARM)/bin:$(PATH)

CC=arm-none-eabi-gcc
CP=arm-none-eabi-g++
OC=arm-none-eabi-objcopy 
LD=arm-none-eabi-ld
AM=armips

CFLAGS=-std=gnu99 -Os -g -mword-relocations -fomit-frame-pointer -ffast-math -fshort-wchar
C9FLAGS=-mcpu=arm946e-s -march=armv5te -mlittle-endian
C11FLAGS=-mcpu=mpcore -mlittle-endian
LDFLAGS=
OCFLAGS=--set-section-flags .bss=alloc,load,contents

OUT_DIR=obj/arm11 bin

ARM11_OBJS=$(patsubst %.c, obj/%.o, $(wildcard arm11/*.c))
ARM11_OBJS+=$(patsubst %.s, obj/%.o, $(wildcard arm11/*.s))
ARM11_OBJS+=$(patsubst %.S, obj/%.o, $(wildcard arm11/*.S))

.PHONY: clean

all: bin/arm11.bin

bin/arm11.bin: $(ARM11_OBJS) | dirs
	$(LD) -T arm11.ld $(ARM11_OBJS) $(LDFLAGS) -o $@.elf
	$(OC) $(OCFLAGS) -O binary $@.elf $@

obj/%.11.o: %.11.c | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C11FLAGS) $< -o $@

obj/%.11.o: %.11.s | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C11FLAGS) -fno-toplevel-reorder $< -o $@

obj/%.11.o: %.11.S | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C11FLAGS) -fno-toplevel-reorder $< -o $@

obj/%.o: %.c | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C9FLAGS) $< -o $@

obj/%.o: %.s | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C9FLAGS) $< -o $@

obj/%.o: %.S | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C9FLAGS) $< -o $@

dirs: ${OUT_DIR}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

clean:
	rm -rf bin/*.elf bin/*.bin obj/*

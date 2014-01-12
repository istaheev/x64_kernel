.PHONY: all image clean

# Input files
C_SRC = $(wildcard src/*.c)
C_OBJS = ${C_SRC:src/%.c=obj/%.o}

S_SRC = $(wildcard src/*.S)
S_OBJS = ${S_SRC:src/%.S=obj/%.o}

OBJS = $(C_OBJS) $(S_OBJS)

# Compilation flags

CC = gcc
CFLAGS = -g \
	     -nostdlib \
         -fno-exceptions \
         -ffreestanding \
         -fno-stack-protector \
         -mno-red-zone \
         -mno-mmx \
         -mno-sse \
         -mno-sse2 \
         -mno-sse3 \
         -mno-3dnow \
         -mcmodel=kernel \
         -Iinclude
LDFLAGS = -nostdlib -z max-page-size=0x1000 -n -T src/linker.ld

# Rules

all: image

bin/kernel: src/linker.ld Makefile $(OBJS)
	ld $(LDFLAGS) -o bin/kernel $(OBJS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.S
	$(CC) $(CFLAGS) -c $< -o $@

image: bin/kernel image/boot/grub/grub.cfg
	cp bin/kernel image/kernel
	grub-mkrescue -o image.iso image

clean:
	-rm image.iso
	-rm bin/kernel
	-rm obj/*.o

dump: bin/kernel
	objdump -D bin/kernel

.PHONY: all image clean

# Input files
INCLUDE = $(wildcard include/*.h)

C_SRC = $(wildcard src/*.c)
C_OBJS = ${C_SRC:src/%.c=obj/%.o}

S_SRC = $(wildcard src/*.S)
S_OBJS = ${S_SRC:src/%.S=obj/%.o}

OBJS = $(C_OBJS) $(S_OBJS)

# Compilation flags

CC = gcc
CFLAGS = -g                   \
	     -std=c99             \
	     -pedantic            \
		 -Wall                \
		 -Wextra              \
	     -nostdlib            \
	     -fomit-frame-pointer \
         -fno-exceptions      \
         -ffreestanding       \
         -fno-stack-protector \
         -mno-red-zone        \
         -mno-mmx             \
         -mno-sse             \
         -mno-sse2            \
         -mno-sse3            \
         -mno-3dnow           \
         -mcmodel=kernel      \
         -Iinclude

LDFLAGS = -n                      \
	      -nostdlib               \
		  -z max-page-size=0x1000 \
		  -T src/linker.ld

# Rules

image: bin/kernel image/boot/grub/grub.cfg
	cp bin/kernel image/kernel
	grub-mkrescue -o image.iso image

bin/kernel: src/linker.ld Makefile $(OBJS)
	ld $(LDFLAGS) -o bin/kernel $(OBJS)

obj/%.o: src/%.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.S $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm image.iso
	-rm bin/kernel
	-rm obj/*.o

all: clean image

dump: bin/kernel
	objdump -D bin/kernel

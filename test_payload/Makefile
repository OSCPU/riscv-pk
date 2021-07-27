NAME = test_payload
ELF = $(NAME)
HOST ?= riscv64-unknown-elf
CC = $(HOST)-gcc
OBJDUMP = $(HOST)-objdump


HDRS = $(shell find -L ./ -name "*.h") \
  ../machine/mcall.h \

C_SRCS = $(shell find -L ./ -name "*.c")
ASM_SRCS = $(shell find -L ./ -name "*.S")
LDSCRIPT = $(NAME).lds


INCLUDE_FLAG = \
  -Iinclude \
  -Itest \
  -Iulib/include \
  -I../machine \

CFLAGS = -O0 -fno-builtin -nostdlib -T $(LDSCRIPT) -Wall -mcmodel=medany $(INCLUDE_FLAG) -nostdinc -g -fvar-tracking


default: $(ELF)

$(ELF): $(ASM_SRCS) $(C_SRCS) $(HDRS) $(LDSCRIPT)
	$(CC) $(CFLAGS) -o $@ $(ASM_SRCS) $(C_SRCS)
	$(OBJDUMP) -d $(ELF) > $(NAME).txt

clean:
	rm -f $(ELF) $(NAME).txt

.PHONY: default clean

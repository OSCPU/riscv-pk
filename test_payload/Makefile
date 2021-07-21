NAME = test_payload
ELF = $(NAME)
HOST ?= riscv64-unknown-elf
CC = $(HOST)-gcc


HDRS = $(shell find -L ./include/ -name "*.h") \
  ../machine/mcall.h \

C_SRCS = $(shell find -L ./ -name "*.c")
ASM_SRCS = $(shell find -L ./ -name "*.S")
LDSCRIPT = $(NAME).lds


INCLUDE_FLAG = \
  -Iinclude \
  -I../machine \

CFLAGS = -O0 -nostdlib -T $(LDSCRIPT) -Wall -mcmodel=medany $(INCLUDE_FLAG) -nostdinc -g -fvar-tracking


default: $(ELF)

$(ELF): $(ASM_SRCS) $(C_SRCS) $(HDRS) $(LDSCRIPT)
	$(CC) $(CFLAGS) -o $@ $(ASM_SRCS) $(C_SRCS)

clean:
	rm -f $(ELF)

.PHONY: default clean
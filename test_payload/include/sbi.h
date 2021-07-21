#ifndef _ASM_RISCV_SBI_H
#define _ASM_RISCV_SBI_H

#include "mcall.h"
#include "type.h"

#define SBI_CALL_1(which, arg0)                               \
    ({                                                        \
        register uintptr_t a0 asm("a0") = (uintptr_t)(arg0);  \
        register uintptr_t a7 asm("a7") = (uintptr_t)(which); \
        asm volatile("ecall"                                  \
                     : "+r"(a0)                               \
                     : "r"(a7)                                \
                     : "memory");                             \
        a0;                                                   \
    })

static inline void sbi_console_putstr(char *str)
{
    while (*str != '\0') {
        SBI_CALL_1(SBI_CONSOLE_PUTCHAR, *str++);
    }
}

#endif

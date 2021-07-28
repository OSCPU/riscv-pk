#ifndef _ASM_RISCV_SBI_H
#define _ASM_RISCV_SBI_H

#include "mcall.h"
#include "type.h"

#define SBI_CALL_0(which)                                     \
    ({                                                        \
        register uintptr_t a7 asm("a7") = (uintptr_t)(which); \
        asm volatile("ecall"                                  \
                     :                                        \
                     : "r"(a7)                                \
                     : "memory");                             \
    })

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

static inline void sbi_console_putchar(int ch)
{
    SBI_CALL_1(SBI_CONSOLE_PUTCHAR, ch);
}

static inline void sbi_set_timer(uint64_t stime_value)
{
#if __riscv_xlen == 32
    SBI_CALL_2(SBI_SET_TIMER, stime_value, stime_value >> 32);
#else
    SBI_CALL_1(SBI_SET_TIMER, stime_value);
#endif
}

enum SBI_READY_FDT_TYPE {
    TIMEBASE,
    SLCR_BADE_ADDR,
    ETHERNET_ADDR,
    PLIC_ADDR,
    NR_IRQS
};
static inline uint32_t sbi_read_fdt(enum SBI_READY_FDT_TYPE type)
{
    return SBI_CALL_1(SBI_READFDT, (int)type);
}

static inline void sbi_shutdown()
{
    SBI_CALL_0(SBI_SHUTDOWN);
}

#endif

#include <os/irq.h>
#include <os/time.h>
#include <os/sched.h>
#include <os/string.h>
#include <kio.h>
#include <kassert.h>
#include <arch/sbi.h>
#include <drivers/screen.h>

handler_t irq_table[IRQC_COUNT];
handler_t exc_table[EXCC_COUNT];
uintptr_t riscv_dtb;

void reset_irq_timer()
{
    // clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    screen_reflush();
    timer_check();
    uint64_t next_tick = get_ticks() + time_base/2000;
    sbi_set_timer(next_tick);
    // screen_reflush();
    do_scheduler();
}

void interrupt_helper(regs_context_t *regs, uint64_t stval, uint64_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    uint64_t interrupt = cause >> 63lu;
    uint64_t _cause = cause & 0x7ffffffffffffffflu;
    if (interrupt) {
        irq_table[_cause](regs, stval,cause);
    } else {
        exc_table[_cause](regs, stval, cause);
    }
}

void handle_int(regs_context_t *regs, uint64_t interrupt, uint64_t cause)
{
    reset_irq_timer();
}

void init_exception()
{
    for (int i = 0; i < IRQC_COUNT; ++i) {
        irq_table[i] = handle_other;
    }
    for (int i = 0; i < EXCC_COUNT; ++i) {
        exc_table[i] = handle_other;
    }
    irq_table[IRQC_S_TIMER] = handle_int;
    exc_table[EXCC_SYSCALL] = handle_syscall;
    exc_table[EXCC_DASICS_SINST_ACCESS] = handle_dasics;
    exc_table[EXCC_DASICS_SLOAD_ACCESS] = handle_dasics;
    exc_table[EXCC_DASICS_SSTORE_ACCESS] = handle_dasics;
    setup_exception();
}

void handle_dasics(regs_context_t *regs, uint64_t stval, uint64_t cause)
{
    switch (cause)
    {
        case EXCC_DASICS_SINST_ACCESS:
            printk("[HANDLE_DASICS]: Detect SInst Access Fault! Skip this instruction!\n");
            break;
        case EXCC_DASICS_SLOAD_ACCESS:
            printk("[HANDLE_DASICS]: Detect SLoad Access Fault! Skip this instruction!\n");
            break;
        case EXCC_DASICS_SSTORE_ACCESS:
            printk("[HANDLE_DASICS]: Detect SStore Access Fault! Skip this instruction!\n");
            break;
        default:
            printk("[HANDLE_DASICS]: Invalid cause 0x%lx detected!\n");
            printk("sstatus: 0x%lx sbadaddr: 0x%lx scause: %lu\n\r",
                regs->sstatus, regs->sbadaddr, regs->scause);
            printk("sepc: 0x%lx\n\r", regs->sepc);
            sbi_shutdown();
            break;
    }

    regs->sepc += 4;
}

void handle_other(regs_context_t *regs, uint64_t stval, uint64_t cause)
{
    printk("[HANDLE_OTHER]: \n");
    static char* reg_name[] = {
        "zero "," ra  "," sp  "," gp  "," tp  ",
        " t0  "," t1  "," t2  ","s0/fp"," s1  ",
        " a0  "," a1  "," a2  "," a3  "," a4  ",
        " a5  "," a6  "," a7  "," s2  "," s3  ",
        " s4  "," s5  "," s6  "," s7  "," s8  ",
        " s9  "," s10 "," s11 "," t3  "," t4  ",
        " t5  "," t6  "
    };
    for (int i = 0; i < 32; i += 3) {
        for (int j = 0; j < 3 && i + j < 32; ++j) {
            printk("%s : %016lx ",reg_name[i+j], regs->regs[i+j]);
        }
        printk("\n\r");
    }
    printk("sstatus: 0x%lx sbadaddr: 0x%lx scause: %lu\n\r",
           regs->sstatus, regs->sbadaddr, regs->scause);
    printk("sepc: 0x%lx\n\r", regs->sepc);
    sbi_shutdown();
}

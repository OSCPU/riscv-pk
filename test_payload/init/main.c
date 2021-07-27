#include <os/irq.h>
#include <os/mm.h>
#include "kio.h"
#include "os/sched.h"
#include "drivers/screen.h"
#include <os/time.h>
#include <os/syscall.h>
#include <os/futex.h>
#include <test.h>
#include "arch/sbi.h"
#include "arch/csr.h"

extern void ret_from_exception();
extern void __global_pointer$();

static void init_pcb()
{
    for (int i = 0; i < NUM_MAX_TASK; ++i) {
        pcb[i].status = TASK_EXITED;
    }

        current_running = &pid0_pcb;
        pid0_pcb.status = TASK_RUNNING;

    do_spawn(&dasics_task, NULL, AUTO_CLEANUP_ON_EXIT);
}

static void init_syscall(void)
{
    // init system call table.
    syscall[SYSCALL_SPAWN] = (long (*)()) do_spawn;
    syscall[SYSCALL_EXIT] = (long (*)()) do_exit;
    syscall[SYSCALL_SLEEP] = (long (*)())do_sleep;
    syscall[SYSCALL_YIELD] = (long (*)()) do_scheduler;

    syscall[SYSCALL_FUTEX_WAIT] = (long (*)())futex_wait;
    syscall[SYSCALL_FUTEX_WAKEUP] = (long (*)())futex_wakeup;
    syscall[SYSCALL_CURSOR] = (long (*)())screen_move_cursor;
    syscall[SYSCALL_REFLUSH] = (long (*)())screen_reflush;
    syscall[SYSCALL_WRITE] = (long (*)())port_write;
    syscall[SYSCALL_GET_TIMEBASE] = (long (*)())get_time_base;
    syscall[SYSCALL_GET_TICK] = (long (*)())get_ticks;
}

int main()
{
    // init Process Control Block
    init_pcb();
    printk("> [INIT] PCB initialization succeeded.\n");

    // read CPU frequency
    // time_base = sbi_read_fdt(TIMEBASE);
    // time_base = 500000000;
    // printk("time_base = %d\n\r", time_base);

    // init futex mechanism
    init_system_futex();
    // init interrupt
    init_exception();
    printk("> [INIT] Interrupt processing initialization succeeded.\n\r");

    // init system call table
    init_syscall();
    printk("> [INIT] System call initialized successfully.\n\r");

    // init screen
    // init_screen();
    // printk("> [INIT] SCREEN initialization succeeded.\n\r");
   
    // Enable interrupt
    // reset_irq_timer();

    do_scheduler();

    return 0;
}

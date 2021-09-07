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
#include "kdasics.h"
#include <kassert.h>

extern void ret_from_exception();
extern void __global_pointer$();

static char ATTR_SLIB_DATA secret[100] = "[SLIB1]: It's the secret!\n";
static char ATTR_SLIB_DATA pub_readonly[100] = "[SLIB1]: Enter dasics_slib1 ...\n";
static char ATTR_SLIB_DATA pub_rwbuffer[100] = "[SLIB1]: It's public rw buffer!\n";

static void ATTR_SLIB_TEXT dasics_slib1(void)
{
    printk(pub_readonly);                 // That's ok
    pub_readonly[15] = 'B';               // raise DasicsSStoreAccessFault (0x15)
    printk(pub_rwbuffer);                 // That's ok

    char temp = secret[3];                // raise DasicsSLoadAccessFault  (0x13)
    secret[3] = temp;                     // raise DasicsSStoreAccessFault (0x15)

    ret_from_exception();                 // raise DasicsSInstrAccessFault (0x11)
    // printk(secret);                       // raise DasicsSLoadAccessFault * 100

    pub_rwbuffer[19] = pub_readonly[12];  // That's ok
    pub_rwbuffer[21] = 'B';               // That's ok
    pub_rwbuffer[22] = 'B';               // That's ok
    printk(pub_rwbuffer);                 // That's ok
}

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

static void init_dasics(void)
{
    extern char _ftext, _etext;
    assert(0 == sbi_modify_smain_bound((ptr_t)&_etext - 0x2UL, (ptr_t)&_ftext));

    // Init dasicsLibCfg for supervisor lib functions, like printk, kstring and screen drivers et.al
    extern char __RODATA_BEGIN__, __RODATA_END__;
    assert(dasics_libcfg_kalloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R,
                (ptr_t)&__RODATA_END__, (ptr_t)&__RODATA_BEGIN__) == 0);  // rodata section

    extern char __SFREEZONE_TEXT_BEGIN__, __SFREEZONE_TEXT_END__;
    assert(dasics_libcfg_kalloc(DASICS_LIBCFG_V | DASICS_LIBCFG_X,
                (ptr_t)&__SFREEZONE_TEXT_END__ - 0x2UL, (ptr_t)&__SFREEZONE_TEXT_BEGIN__) == 1);  // freezone text

    extern char __SFREEZONE_DATA_BEGIN__, __SFREEZONE_DATA_END__;
    assert(dasics_libcfg_kalloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W,
                (ptr_t)&__SFREEZONE_DATA_END__, (ptr_t)&__SFREEZONE_DATA_BEGIN__) == 2);  // freezone data

    assert(dasics_libcfg_kalloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W,
                (ptr_t)(INIT_KERNEL_STACK + PAGE_SIZE), (ptr_t)INIT_KERNEL_STACK) == 3);  // stack space for the pid0 process

    dasics_init_smaincall((ptr_t)&dasics_smaincall);

    // printk("DEBUG: pub_readonly (0x%lx ~ 0x%lx), pub_rwbuffer (0x%lx ~ 0x%lx), secret (0x%x ~ 0x%x)\n",
    //     pub_readonly, pub_readonly + 100, pub_rwbuffer, pub_rwbuffer + 100, secret, secret + 100);

    // printk("DEBUG: .rodata range from 0x%lx to 0x%lx, .sfreezonetext range from 0x%lx to 0x%lx\n",
    //     (ptr_t)&__RODATA_BEGIN__, (ptr_t)&__RODATA_END__, (ptr_t)&__SFREEZONE_TEXT_BEGIN__, (ptr_t)&__SFREEZONE_TEXT_END__ - 0x2UL);
    // printk("DEBUG: .sfreezonedata range from 0x%lx to 0x%lx, stack space range from 0x%lx to 0x%lx\n",
    //     (ptr_t)&__SFREEZONE_DATA_BEGIN__, (ptr_t)&__SFREEZONE_DATA_END__, (ptr_t)INIT_KERNEL_STACK, (ptr_t)(INIT_KERNEL_STACK + PAGE_SIZE));
}

int main()
{
    // init DASICS mechanism
    init_dasics();
    printk("> [INIT] Dasics mechanism initialized successfully.\n\r");

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

    // DASICS MODES TEST
    int idx0 = dasics_libcfg_kalloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R                  , (ptr_t)(pub_readonly + 100), (ptr_t)pub_readonly);
    int idx1 = dasics_libcfg_kalloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (ptr_t)(pub_rwbuffer + 100), (ptr_t)pub_rwbuffer);
    int idx2 = dasics_libcfg_kalloc(DASICS_LIBCFG_V                                    , (ptr_t)(      secret + 100), (ptr_t)secret);

    dasics_slib1();

    dasics_libcfg_kfree(idx2);
    dasics_libcfg_kfree(idx1);
    dasics_libcfg_kfree(idx0);

    do_scheduler();

    return 0;
}

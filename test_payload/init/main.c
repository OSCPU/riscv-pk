#include <os/mm.h>
#include "kio.h"
#include "os/sched.h"
#include "arch/csr.h"

extern void ret_from_exception();
extern void __global_pointer$();

static void init_pcb_stack(
    ptr_t kernel_stack, ptr_t user_stack, ptr_t entry_point,
    pcb_t *pcb)
{
    regs_context_t *pt_regs =
        (regs_context_t *)(kernel_stack - sizeof(regs_context_t));
    for (int i = 0; i < 32; ++i) {
        pt_regs->regs[i] = 0;
    }
    // ra = ret_from_exception
    pt_regs->regs[2] = (reg_t)user_stack;
    pt_regs->regs[3] = (reg_t)__global_pointer$;
    pt_regs->sepc    = entry_point;
    pt_regs->sstatus = 0;
    if (pcb->type == KERNEL_PROCESS ||
        pcb->type == KERNEL_THREAD) {
        pt_regs->sstatus |= SR_SPP | SR_SPIE;
    }
    // set sp to simulate return from switch_to
    ptr_t new_ksp = kernel_stack - sizeof(regs_context_t) -
                    sizeof(switchto_context_t);
    pcb->kernel_sp = new_ksp;

    switchto_context_t *st_regs = (switchto_context_t *)new_ksp;
    for (int i = 0; i < 14; ++i) {
        st_regs->regs[i] = 0;
    }
    st_regs->regs[0] = (reg_t)ret_from_exception;
    // printk("%x %x\n",entry_point, (ptr_t)printk_task1);
    // st_regs->regs[0] = entry_point;
    st_regs->regs[1] = new_ksp;
}

static void init_pcb()
{
    static struct task_info* task_group[16] = {0};

    for (int i = 0; i < NUM_MAX_TASK; ++i) {
        if (task_group[i] != NULL) {
            pcb[i].pid       = i + 1;
            pcb[i].type      = task_group[i]->type;
            pcb[i].status    = TASK_READY;
            pcb[i].kernel_sp = allocPage(1) + PAGE_SIZE;
            pcb[i].user_sp   = allocPage(1) + PAGE_SIZE;
            pcb[i].preempt_count = 0;

            init_pcb_stack(
                pcb[i].kernel_sp, pcb[i].user_sp,
                task_group[i]->entry_point, &pcb[i]);

            pcb[i].cursor_x = 1;
            pcb[i].cursor_y = 1;

            list_add_tail(&pcb[i].list, &ready_queue);
        }
    }
    current_running = &pid0_pcb;
    pid0_pcb.status = TASK_RUNNING;
}

int main()
{
    printk("Hello OS!\n");
    // init Process Control Block
    init_pcb();
    printk("> [INIT] PCB initialization succeeded.\n");

    return 0;
}

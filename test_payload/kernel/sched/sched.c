#include <os/list.h>
#include <os/mm.h>
#include <os/lock.h>
#include <os/sched.h>
#include <os/time.h>
#include <os/irq.h>
#include <drivers/screen.h>
#include <kio.h>
#include <kassert.h>
#include "arch/csr.h"

extern void ret_from_exception();
extern void __global_pointer$();

pcb_t pcb[NUM_MAX_TASK];
const ptr_t pid0_stack = INIT_KERNEL_STACK + PAGE_SIZE;
pcb_t pid0_pcb = {
    .pid = 0,
    .kernel_sp = (ptr_t)pid0_stack,
    .user_sp = (ptr_t)pid0_stack,
    .preempt_count = 0
};

LIST_HEAD(ready_queue);

/* current running task PCB */
pcb_t * volatile current_running;

/* global process id */
pid_t process_id = 1;

void do_scheduler(void)
{
    pcb_t *prev_running = current_running;
    // Modify the current_running pointer.
    if (!list_empty(&ready_queue)) {
        current_running = list_entry(ready_queue.next, pcb_t, list);
    } else {
        if (current_running->status == TASK_RUNNING) return;    // do nothing
        else current_running = &pid0_pcb;   // no process remaining, yield to kernel
    }
    if (prev_running != current_running) {
        // prev_running's status can be TASK_RUNNINT or TASK_BLOCKED
        if (prev_running->status == TASK_RUNNING) {
            prev_running->status = TASK_READY;
            if (prev_running != &pid0_pcb) {
                list_add_tail(&prev_running->list, &ready_queue);
            }
        }
        assert(current_running->status == TASK_READY ||
                current_running->status == TASK_RUNNING);
        current_running->status = TASK_RUNNING;
        list_del(&current_running->list);
        /*
        vt100_move_cursor(current_running->cursor_x,
                            current_running->cursor_y);
        */
        screen_cursor_x = current_running->cursor_x;
        screen_cursor_y = current_running->cursor_y;
        switch_to(prev_running, current_running);
    }
}

void do_sleep(uint32_t sleep_time)
{
    current_running->status = TASK_BLOCKED;
    current_running->list.next = NULL;
    current_running->list.prev = NULL;
    timer_create((TimerCallback)do_unblock, &current_running->list,
                 get_ticks() + sleep_time * time_base);
    do_scheduler();
}

void do_block(list_node_t *pcb_node, list_head *queue)
{
    // block the pcb task into the block queue
    pcb_t* _pcb = list_entry(pcb_node, pcb_t, list);
    if (_pcb->status == TASK_READY ||
        _pcb->status == TASK_RUNNING) {
        _pcb->status = TASK_BLOCKED;
        list_del(pcb_node);
        list_add_tail(pcb_node, queue);
    } else {
        assert(0);
    }
}

void do_unblock(list_node_t *pcb_node)
{
    // unblock the `pcb` from the block queue
    pcb_t* _pcb = list_entry(pcb_node, pcb_t, list);
    if (_pcb->status == TASK_BLOCKED) {
        _pcb->status = TASK_READY;
        list_del(pcb_node);
        list_add_tail(pcb_node, &ready_queue);
    } else {
        printk("pid: %d status: %d\n\r",_pcb->pid, _pcb->status);
        assert(0);
    }
}

static void init_pcb_stack(
    ptr_t kernel_stack, ptr_t user_stack, ptr_t entry_point,
    pcb_t *pcb, void* arg)
{
    regs_context_t *pt_regs =
        (regs_context_t *)(kernel_stack - sizeof(regs_context_t));
    for (int i = 0; i < 32; ++i) {
        pt_regs->regs[i] = 0;
    }
    // ra = ret_from_exception
    pt_regs->regs[2] = (reg_t)user_stack;
    pt_regs->regs[3] = (reg_t)__global_pointer$;
    pt_regs->regs[10] = (reg_t)arg;
    pt_regs->sepc    = entry_point;
    pt_regs->sstatus = 0;
    if (pcb->type == KERNEL_PROCESS ||
        pcb->type == KERNEL_THREAD) {
        pt_regs->sstatus |= SR_SPP | SR_SPIE;
    } else {
        pt_regs->sstatus |= SR_SPIE;
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
    st_regs->regs[1] = new_ksp;
}

static void set_pcb(
    pid_t pid, pcb_t *my_pcb, task_info_t *my_task, void *arg)
{
    my_pcb->pid               = pid;
    my_pcb->type              = my_task->type;
    my_pcb->status            = TASK_READY;
    my_pcb->kernel_stack_base = allocPage(1);
    my_pcb->user_stack_base   = allocPage(1);
    my_pcb->kernel_sp     = my_pcb->kernel_stack_base + PAGE_SIZE;
    my_pcb->user_sp       = my_pcb->user_stack_base + PAGE_SIZE;
    my_pcb->preempt_count = 0;

    init_pcb_stack(
        my_pcb->kernel_sp, my_pcb->user_sp,
        my_task->entry_point, my_pcb, arg);

    my_pcb->cursor_x = 1;
    my_pcb->cursor_y = 1;

    init_list_head(&my_pcb->wait_list);
    list_add_tail(&my_pcb->list, &ready_queue);
}

static void clean_up_pcb(pcb_t *_pcb)
{
    /* [1] remove from queue */
    list_del(&_pcb->list);

    /* [2] free stack space */
    freePage(_pcb->kernel_stack_base, 1);
    freePage(_pcb->user_stack_base, 1);

    /* [3] set status = TASK_EXIT */
    if (_pcb->mode == AUTO_CLEANUP_ON_EXIT) {
        _pcb->status = TASK_EXITED;
    } else {
        _pcb->status = TASK_ZOMBIE;
    }

    while (!list_empty(&_pcb->wait_list)) {
        list_node_t *p = _pcb->wait_list.next;
        do_unblock(p);
        _pcb->status = TASK_EXITED;
    }
}

void do_exit(void)
{
    clean_up_pcb(current_running);
    do_scheduler();
}

pid_t do_spawn(task_info_t *task, void* arg, spawn_mode_t mode)
{
    int i;
    pid_t ret = -1;

    for (i = 0; i < NUM_MAX_TASK; i++)
    {
        if (pcb[i].status == TASK_EXITED)
        {
            ret = process_id++;
            set_pcb(ret, &pcb[i], task, arg);
            pcb[i].mode = mode;
            break;
        }
    }
    return ret;
}

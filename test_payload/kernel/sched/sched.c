#include <os/list.h>
#include <os/mm.h>
#include <os/lock.h>
#include <os/sched.h>
#include <os/time.h>
#include <os/irq.h>
#include <drivers/screen.h>
#include <kio.h>
#include <kassert.h>

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
    // TODO schedule
    // Modify the current_running pointer.
    if (!list_empty(&ready_queue)) {
        pcb_t *prev_running = current_running;
        current_running = list_entry(ready_queue.next, pcb_t, list);
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
            vt100_move_cursor(current_running->cursor_x,
                              current_running->cursor_y);
            screen_cursor_x = current_running->cursor_x;
            screen_cursor_y = current_running->cursor_y;
            switch_to(prev_running, current_running);
        }
    }
    // printk("begin switch\n\r");
    // switch_to(current_running, current_running);
    // printk("end switch\n\r");
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

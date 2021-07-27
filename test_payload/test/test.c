#include "test.h"

struct task_info dasics_task = {(ptr_t)&dasics_example, USER_PROCESS};

/* [TASK1] [TASK3] task group to test do_scheduler() */
// do_scheduler() annotations are required for non-robbed scheduling
/*
struct task_info task2_1 = {(ptr_t)&printk_task1, KERNEL_THREAD};
struct task_info task2_2 = {(ptr_t)&printk_task2, KERNEL_THREAD};
struct task_info task2_3 = {(ptr_t)&drawing_task1, KERNEL_THREAD};
struct task_info *sched1_tasks[16] = {&task2_1, &task2_2};
int num_sched1_tasks = 2;

struct task_info task2_8 = {(ptr_t)&printf_task1, USER_PROCESS};
struct task_info task2_9 = {(ptr_t)&printf_task2, USER_PROCESS};
struct task_info task2_10 = {(ptr_t)&drawing_task2, USER_PROCESS};
struct task_info *sched2_tasks[16] = {&task2_8, &task2_9};
int num_sched2_tasks = 2;
*/

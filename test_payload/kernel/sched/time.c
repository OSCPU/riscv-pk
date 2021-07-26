#include <os/time.h>
#include <os/mm.h>
#include <os/irq.h>
#include <type.h>

LIST_HEAD(timers);

uint64_t time_elapsed = 0;
uint32_t time_base = 0;
uint64_t MHZ = 10;

static LIST_HEAD(free_timers);

static timer_t* alloc_timer()
{
    // if we have free timer, then return a free timer
    // otherwise, return a new timer which is allocated by calling kmalloc
    timer_t *ret = NULL;
    if (!list_empty(&free_timers)) {
        ret = list_entry(free_timers.next, timer_t, list);
        list_del(&ret->list);
    } else {
        ret = (timer_t*)kmalloc(sizeof(timer_t));
    }

    // initialize the timer
    ret->list.next = NULL;
    ret->list.prev = NULL;
    ret->timeout_tick = 0;
    ret->callback_func = (TimerCallback) NULL;
    ret->parameter = NULL;
    return ret;
}

static void free_timer(timer_t *timer)
{
    list_add_tail(&timer->list, &free_timers);
}

void timer_create(TimerCallback func, void* parameter, uint64_t tick)
{
    disable_preempt();

    timer_t *timer = alloc_timer();
    timer->callback_func = func;
    timer->parameter = parameter;
    timer->timeout_tick = tick;

    list_node_t *p = timers.next;
    for (; p != &timers; p = p->next) {
        timer_t *t = list_entry(p, timer_t, list);
        if (t->timeout_tick > timer->timeout_tick) {
            break;
        }
    }
    list_add_tail(&timer->list, p);

    enable_preempt();
}

void timer_check()
{
    disable_preempt();
    if (!list_empty(&timers)) {
        uint64_t ticks = get_ticks();
        timer_t *timer = list_entry(timers.next, timer_t, list);
        while (timer->timeout_tick <= ticks) {
            timer->callback_func(timer->parameter);
            list_del(&timer->list);
            free_timer(timer);

            if (list_empty(&timers)) break;

            timer = list_entry(timers.next, timer_t, list);
        }
    }
    enable_preempt();
}

uint64_t get_ticks()
{
    __asm__ __volatile__(
        "rdtime %0"
        : "=r"(time_elapsed));
    return time_elapsed;
}

uint64_t get_timer()
{
    return get_ticks() / time_base;
}

uint64_t get_time_base()
{
    return time_base;
}

void latency(uint64_t time)
{
    uint64_t begin_time = get_timer();

    while (get_timer() - begin_time < time)
    {
    };
    return;
}

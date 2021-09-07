#ifndef _STDDASICS_H_
#define _STDDASICS_H_

#include <stdint.h>
#include <stdattr.h>

#define DASICS_LIBCFG_WIDTH 8
#define DASICS_LIBCFG_MASK  0xfUL
#define DASICS_LIBCFG_V     0x8UL
#define DASICS_LIBCFG_X     0x4UL
#define DASICS_LIBCFG_R     0x2UL
#define DASICS_LIBCFG_W     0x1UL

typedef enum {
    UMAINCALL_EXIT,
    UMAINCALL_YIELD,
    UMAINCALL_SLEEP,
    UMAINCALL_WRITE,
    UMAINCALL_REFLUSH,
    UMAINCALL_MOVE_CURSOR,
    UMAINCALL_FUTEX_WAIT,
    UMAINCALL_FUTEX_WAKEUP,
    UMAINCALL_GET_TIMEBASE,
    UMAINCALL_GET_TICK
} UmaincallTypes;

void     ATTR_UMAIN_TEXT dasics_init_umaincall(uint64_t entry);
uint64_t ATTR_UMAIN_TEXT dasics_umaincall(UmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2);
void     ATTR_UMAIN_TEXT dasics_ufault_entry(void);
int32_t  ATTR_UMAIN_TEXT dasics_libcfg_alloc(uint64_t cfg, uint64_t hi, uint64_t lo);
int32_t  ATTR_UMAIN_TEXT dasics_libcfg_free(int32_t idx);
uint32_t ATTR_UMAIN_TEXT dasics_libcfg_get(int32_t idx);

#endif
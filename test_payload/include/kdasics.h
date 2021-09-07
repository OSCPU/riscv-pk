#ifndef INCLUDE_DASICS_H_
#define INCLUDE_DASICS_H_

#include "type.h"
#include "kattr.h"

#define DASICS_LIBCFG_WIDTH 8
#define DASICS_LIBCFG_MASK  0xfUL
#define DASICS_LIBCFG_V     0x8UL
#define DASICS_LIBCFG_X     0x4UL
#define DASICS_LIBCFG_R     0x2UL
#define DASICS_LIBCFG_W     0x1UL

typedef enum {
    SMAINCALL_DISABLE_PREEMPT,
    SMAINCALL_ENABLE_PREEMPT,
    SMAINCALL_WRITE,
    SMAINCALL_WRITE_CH,
    SMAINCALL_LOAD_CURSOR_X,
    SMAINCALL_LOAD_CURSOR_Y,
    SMAINCALL_STORE_CURSOR
} SmaincallTypes;

void     ATTR_SMAIN_TEXT dasics_init_umain_bound(uint64_t cfg, uint64_t hi, uint64_t lo);
void     ATTR_SMAIN_TEXT dasics_init_smaincall(uint64_t entry);
uint64_t ATTR_SMAIN_TEXT dasics_smaincall(SmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2);
int32_t  ATTR_SMAIN_TEXT dasics_libcfg_kalloc(uint64_t cfg, uint64_t hi, uint64_t lo);
int32_t  ATTR_SMAIN_TEXT dasics_libcfg_kfree(int32_t idx);
uint32_t ATTR_SMAIN_TEXT dasics_libcfg_kget(int32_t idx);

#endif
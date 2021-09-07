#include "type.h"
#include "kcsr.h"
#include "kattr.h"
#include "kdasics.h"
#include <os/sched.h>
#include <os/irq.h>
#include <arch/common.h>
#include <kio.h>

void ATTR_SMAIN_TEXT dasics_init_umain_bound(uint64_t cfg, uint64_t hi, uint64_t lo)
{
    kwrite_csr(0x5c0, cfg);  // DasicsUMainCfg
    kwrite_csr(0x5c1, hi);   // DasicsUMainBoundHi
    kwrite_csr(0x5c2, lo);   // DasicsUMainBoundLo
}

void ATTR_SMAIN_TEXT dasics_init_smaincall(uint64_t entry)
{
    kwrite_csr(0x8a3, entry);  // DasicsMaincallEntry
}

uint64_t ATTR_SMAIN_TEXT dasics_smaincall(SmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
    uint64_t dasics_return_pc = kread_csr(0x8a4);
    uint64_t dasics_free_zone_return_pc = kread_csr(0x8a5);

    uint64_t retval = 0;

    switch (type)
    {
        case SMAINCALL_DISABLE_PREEMPT:
            disable_preempt();
            break;
        case SMAINCALL_ENABLE_PREEMPT:
            enable_preempt();
            break;
        case SMAINCALL_WRITE:
            port_write((char *)arg0);
            break;
        case SMAINCALL_WRITE_CH:
            port_write_ch((char)arg0);
            break;
        case SMAINCALL_LOAD_CURSOR_X:
            retval = (uint64_t)current_running->cursor_x;
            break;
        case SMAINCALL_LOAD_CURSOR_Y:
            retval = (uint64_t)current_running->cursor_y;
            break;
        case SMAINCALL_STORE_CURSOR:
            current_running->cursor_x = (int)arg0;
            current_running->cursor_y = (int)arg1;
            break;
        default:
            printk("Warning: Invalid smaincall number %u!\n", type);
            break;
    }

    kwrite_csr(0x8a4, dasics_return_pc);
    kwrite_csr(0x8a5, dasics_free_zone_return_pc);

    // TODO: Use compiler to optimize such ugly code in the future ...
    asm volatile("mv        a0, a5\n"\
                 "ld        ra, 88(sp)\n"\
                 "ld        s0, 80(sp)\n"\
                 "addi      sp, sp, 96\n"\
                 "pulpret   x0,  0, x1\n"\
                 "nop");

    return retval;
}

int32_t ATTR_SMAIN_TEXT dasics_libcfg_kalloc(uint64_t cfg, uint64_t hi, uint64_t lo)
{
    uint64_t libcfg0 = kread_csr(0x881);  // DasicsLibCfg0
    uint64_t libcfg1 = kread_csr(0x882);  // DasicsLibCfg1

    int32_t max_cfgs = DASICS_LIBCFG_WIDTH << 1;
#ifndef RV32
    int32_t step = 8;  // rv64
#else
    int32_t step = 4;  // rv32
#endif

    for (int32_t idx = 0; idx < max_cfgs; ++idx)
    {
        int choose_libcfg0 = (idx < DASICS_LIBCFG_WIDTH);
        int32_t _idx = choose_libcfg0 ? idx : idx - DASICS_LIBCFG_WIDTH;
        uint64_t curr_cfg = ((choose_libcfg0 ? libcfg0 : libcfg1) >> (_idx * step)) & DASICS_LIBCFG_MASK;

        if ((curr_cfg & DASICS_LIBCFG_V) == 0)  // Find avaliable cfg
        {
            if (choose_libcfg0)
            {
                libcfg0 &= ~(DASICS_LIBCFG_MASK << (_idx * step));
                libcfg0 |= (cfg & DASICS_LIBCFG_MASK) << (_idx * step);
                kwrite_csr(0x881, libcfg0);  // DasicsLibCfg0
            }
            else
            {
                libcfg1 &= ~(DASICS_LIBCFG_MASK << (_idx * step));
                libcfg1 |= (cfg & DASICS_LIBCFG_MASK) << (_idx * step);
                kwrite_csr(0x882, libcfg1);  // DasicsLibCfg1
            }

            // Write DASICS boundary csrs
            switch (idx)
            {
                case 0:
                    kwrite_csr(0x883, hi);  // DasicsLibBound0
                    kwrite_csr(0x884, lo);  // DasicsLibBound1
                    break;
                case 1:
                    kwrite_csr(0x885, hi);  // DasicsLibBound2
                    kwrite_csr(0x886, lo);  // DasicsLibBound3
                    break;
                case 2:
                    kwrite_csr(0x887, hi);  // DasicsLibBound4
                    kwrite_csr(0x888, lo);  // DasicsLibBound5
                    break;
                case 3:
                    kwrite_csr(0x889, hi);  // DasicsLibBound6
                    kwrite_csr(0x88a, lo);  // DasicsLibBound7
                    break;
                case 4:
                    kwrite_csr(0x88b, hi);  // DasicsLibBound8
                    kwrite_csr(0x88c, lo);  // DasicsLibBound9
                    break;
                case 5:
                    kwrite_csr(0x88d, hi);  // DasicsLibBound10
                    kwrite_csr(0x88e, lo);  // DasicsLibBound11
                    break;
                case 6:
                    kwrite_csr(0x88f, hi);  // DasicsLibBound12
                    kwrite_csr(0x890, lo);  // DasicsLibBound13
                    break;
                case 7:
                    kwrite_csr(0x891, hi);  // DasicsLibBound14
                    kwrite_csr(0x892, lo);  // DasicsLibBound15
                    break;
                case 8:
                    kwrite_csr(0x893, hi);  // DasicsLibBound16
                    kwrite_csr(0x894, lo);  // DasicsLibBound17
                    break;
                case 9:
                    kwrite_csr(0x895, hi);  // DasicsLibBound18
                    kwrite_csr(0x896, lo);  // DasicsLibBound19
                    break;
                case 10:
                    kwrite_csr(0x897, hi);  // DasicsLibBound20
                    kwrite_csr(0x898, lo);  // DasicsLibBound21
                    break;
                case 11:
                    kwrite_csr(0x899, hi);  // DasicsLibBound22
                    kwrite_csr(0x89a, lo);  // DasicsLibBound23
                    break;
                case 12:
                    kwrite_csr(0x89b, hi);  // DasicsLibBound24
                    kwrite_csr(0x89c, lo);  // DasicsLibBound25
                    break;
                case 13:
                    kwrite_csr(0x89d, hi);  // DasicsLibBound26
                    kwrite_csr(0x89e, lo);  // DasicsLibBound27
                    break;
                case 14:
                    kwrite_csr(0x89f, hi);  // DasicsLibBound28
                    kwrite_csr(0x8a0, lo);  // DasicsLibBound29
                    break;
                default:
                    kwrite_csr(0x8a1, hi);  // DasicsLibBound30
                    kwrite_csr(0x8a2, lo);  // DasicsLibBound31
                    break;
            }

            return idx;
        }
    }

    return -1;
}

int32_t ATTR_SMAIN_TEXT dasics_libcfg_kfree(int32_t idx)
{
    if (idx < 0 || idx >= (DASICS_LIBCFG_WIDTH << 1))
    {
        return -1;
    }

#ifndef RV32
    int32_t step = 8;  // rv64
#else
    int32_t step = 4;  // rv32
#endif

    int choose_libcfg0 = (idx < DASICS_LIBCFG_WIDTH);
    int32_t _idx = choose_libcfg0 ? idx : idx - DASICS_LIBCFG_WIDTH;

    uint64_t libcfg = choose_libcfg0 ? kread_csr(0x881):  // DasicsLibCfg0
                                       kread_csr(0x882);  // DasicsLibCfg1
    libcfg &= ~(DASICS_LIBCFG_V << (_idx * step));

    if (choose_libcfg0)
    {
        kwrite_csr(0x881, libcfg);  // DasicsLibCfg0
    }
    else
    {
        kwrite_csr(0x882, libcfg);  // DasicsLibCfg1
    }

    return 0;
}

uint32_t dasics_libcfg_kget(int32_t idx)
{
    if (idx < 0 || idx >= (DASICS_LIBCFG_WIDTH << 1))
    {
        return -1;
    }

#ifndef RV32
    int32_t step = 8;  // rv64
#else
    int32_t step = 4;  // rv32
#endif

    int choose_libcfg0 = (idx < DASICS_LIBCFG_WIDTH);
    int32_t _idx = choose_libcfg0 ? idx : idx - DASICS_LIBCFG_WIDTH;

    uint64_t libcfg = choose_libcfg0 ? kread_csr(0x881):  // DasicsLibCfg0
                                       kread_csr(0x882);  // DasicsLibCfg1

    return (libcfg >> (_idx * step)) & DASICS_LIBCFG_MASK;
}
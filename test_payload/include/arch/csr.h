/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Regents of the University of California
 */

#ifndef CSR_H
#define CSR_H

/* Status register flags */
#define SR_SIE    0x00000002 /* Supervisor Interrupt Enable */
#define SR_SPIE   0x00000020 /* Previous Supervisor IE */
#define SR_SPP    0x00000100 /* Previously Supervisor */
#define SR_SUM    0x00040000 /* Supervisor User Memory Access */

#define SR_FS           0x00006000 /* Floating-point Status */
#define SR_FS_OFF       0x00000000
#define SR_FS_INITIAL   0x00002000
#define SR_FS_CLEAN     0x00004000
#define SR_FS_DIRTY     0x00006000

#define SR_XS           0x00018000 /* Extension Status */
#define SR_XS_OFF       0x00000000
#define SR_XS_INITIAL   0x00008000
#define SR_XS_CLEAN     0x00010000
#define SR_XS_DIRTY     0x00018000

#define SR_SD           0x8000000000000000 /* FS/XS dirty */

/* SATP flags */
#define SATP_PPN        0x00000FFFFFFFFFFF
#define SATP_MODE_39    0x8000000000000000
#define SATP_MODE       SATP_MODE_39

/* SCAUSE */
#define SCAUSE_IRQ_FLAG   (1 << 63)

#define IRQ_U_SOFT		0
#define IRQ_S_SOFT		1
#define IRQ_M_SOFT		3
#define IRQ_U_TIMER		4
#define IRQ_S_TIMER		5
#define IRQ_M_TIMER		7
#define IRQ_U_EXT		8
#define IRQ_S_EXT		9
#define IRQ_M_EXT		11

#define EXC_INST_MISALIGNED	0
#define EXC_INST_ACCESS		1
#define EXC_BREAKPOINT		3
#define EXC_LOAD_ACCESS		5
#define EXC_STORE_ACCESS	7
#define EXC_SYSCALL		8
#define EXC_INST_PAGE_FAULT	12
#define EXC_LOAD_PAGE_FAULT	13
#define EXC_STORE_PAGE_FAULT	15

/* SIE (Interrupt Enable) and SIP (Interrupt Pending) flags */
#define SIE_SSIE    (0x1 << IRQ_S_SOFT)
#define SIE_STIE    (0x1 << IRQ_S_TIMER)
#define SIE_SEIE    (0x1 << IRQ_S_EXT)

#define CSR_CYCLE           0xc00
#define CSR_TIME            0xc01
#define CSR_INSTRET         0xc02
#define CSR_SSTATUS         0x100
#define CSR_SIE             0x104
#define CSR_STVEC           0x105
#define CSR_SCOUNTEREN      0x106
#define CSR_SSCRATCH        0x140
#define CSR_SEPC            0x141
#define CSR_SCAUSE          0x142
#define CSR_STVAL           0x143
#define CSR_SIP             0x144
#define CSR_SATP            0x180
#define CSR_CYCLEH          0xc80
#define CSR_TIMEH           0xc81
#define CSR_INSTRETH        0xc82

#define CSR_USTATUS         0x000
#define CSR_UIE             0x004
#define CSR_UTVEC           0x005
#define CSR_USCRATCH        0x040
#define CSR_UEPC            0x041
#define CSR_UCAUSE          0x042
#define CSR_UTVAL           0x043
#define CSR_UIP             0x044

/* DASICS csrs */
#define CSR_DUMCFG          0x5c0
#define CSR_DUMBOUNDHI      0x5c1
#define CSR_DUMBOUNDLO      0x5c2

#define CSR_DLCFG0          0x881
#define CSR_DLCFG1          0x882
#define CSR_DLBOUND0        0x883
#define CSR_DLBOUND1        0x884
#define CSR_DLBOUND2        0x885
#define CSR_DLBOUND3        0x886
#define CSR_DLBOUND4        0x887
#define CSR_DLBOUND5        0x888
#define CSR_DLBOUND6        0x889
#define CSR_DLBOUND7        0x88a
#define CSR_DLBOUND8        0x88b
#define CSR_DLBOUND9        0x88c
#define CSR_DLBOUND10       0x88d
#define CSR_DLBOUND11       0x88e
#define CSR_DLBOUND12       0x88f
#define CSR_DLBOUND13       0x890
#define CSR_DLBOUND14       0x891
#define CSR_DLBOUND15       0x892
#define CSR_DLBOUND16       0x893
#define CSR_DLBOUND17       0x894
#define CSR_DLBOUND18       0x895
#define CSR_DLBOUND19       0x896
#define CSR_DLBOUND20       0x897
#define CSR_DLBOUND21       0x898
#define CSR_DLBOUND22       0x899
#define CSR_DLBOUND23       0x89a
#define CSR_DLBOUND24       0x89b
#define CSR_DLBOUND25       0x89c
#define CSR_DLBOUND26       0x89d
#define CSR_DLBOUND27       0x89e
#define CSR_DLBOUND28       0x89f
#define CSR_DLBOUND29       0x8a0
#define CSR_DLBOUND30       0x8a1
#define CSR_DLBOUND31       0x8a2

#define CSR_DMAINCALL       0x8a3
#define CSR_DRETURNPC       0x8a4
#define CSR_DFZRETURN       0x8a5

#endif /* CSR_H */

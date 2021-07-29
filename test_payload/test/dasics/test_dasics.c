#include <stdio.h>
#include "stdcsr.h"
#include "sys/syscall.h"

#define STACK_TOP 0x80503000UL
#define STACK_BOTTOM 0x80501000UL

static char secret[100] = "It's the secret!\n";
static char pub_readonly[100] = "Info: Enter Lib1 ...\n";
static char pub_rwbuffer[100] = "It's public rw buffer!\n";

static char main_prompt1[100] = "DasicsMainCfg: 0x%lx\n";
static char main_prompt2[100] = "Info: Ready to enter dasics_lib1.\n";
static char main_prompt3[100] = "DasicsReturnPC: 0x%lx\n";
static char main_prompt4[100] = "DasicsFreeZonePC: 0x%lx\n";

static char maincall_info[100] = "Info: Haved entered maincall zone? (f)\n";

static void dasics_maincall(void);
void dasics_dummy(void);

static void dasics_lib1(void) {
    printf(pub_readonly);                 // That's ok
    printf(pub_rwbuffer);                 // That's ok
    pub_rwbuffer[10] = pub_readonly[12];  // That's ok
    pub_rwbuffer[12] = 'B';               // That's ok
    pub_rwbuffer[13] = 'B';               // That's ok
    printf(pub_rwbuffer);                 // That's ok
    // pub_readonly[15] = 0x1;               // raise DasicsStoreAccessFault
    // pub_rwbuffer[0] = secret[0];          // raise DasicsLoadAccessFault
    // printf(secret);                       // That's ok, but seems a little weird?

    dasics_maincall();

    // unsigned long dasicsMainCfg = read_csr(0x880);  // raise illegalInstr exception
    // write_csr(0x882, 0x0);  // raise illegalInstr exception, too
}

void dasics_main(void) {
    write_csr(0x881, 0x0a0b0c0c080b0a0bUL);     // DasicsLibCfg0, LibBound  0 ~ 15
    write_csr(0x882, 0x0b0b0b0bUL);             // DasicsLibCfg1, LibBound 16 ~ 31
    write_csr(0x883, STACK_TOP);                // DasicsLibBound0
    write_csr(0x884, STACK_BOTTOM);             // DasicsLibBound1
    write_csr(0x885, pub_readonly + 100);       // DasicsLibBound2
    write_csr(0x886, pub_readonly);             // DasicsLibBound3
    write_csr(0x887, pub_rwbuffer + 100);       // DasicsLibBound4
    write_csr(0x888, pub_rwbuffer);             // DasicsLibBound5
    write_csr(0x889, secret + 100);             // DasicsLibBound6
    write_csr(0x88a, secret);                   // DasicsLibBound7
    write_csr(0x88b, 0x802033e6UL);             // DasicsLibBound8, BoundHi of string.c, printf.c and syscall.c
    write_csr(0x88c, (uint64_t)&dasics_dummy);  // DasicsLibBound9, BoundLo of string.c, printf.c and syscall.c
    write_csr(0x88d, 0x802001f8UL);             // DasicsLibBound10, BoundHi of invoke_syscall
    write_csr(0x88e, 0x802001ecUL);             // DasicsLibBound11, BoundLo of invoke_syscall
    write_csr(0x88f, main_prompt1 + 100);       // DasicsLibBound12
    write_csr(0x890, main_prompt1);             // DasicsLibBound13
    write_csr(0x891, main_prompt2 + 100);       // DasicsLibBound14
    write_csr(0x892, main_prompt2);             // DasicsLibBound15
    write_csr(0x893, main_prompt3 + 100);       // DasicsLibBound16
    write_csr(0x894, main_prompt3);             // DasicsLibBound17
    write_csr(0x895, main_prompt4 + 100);       // DasicsLibBound18
    write_csr(0x896, main_prompt4);             // DasicsLibBound19
    write_csr(0x897, 0x80203a00UL);             // DasicsLibBound20, may be the bound of some lib variables?
    write_csr(0x898, 0x80203500UL);             // DasicsLibBound21, may be the bound of some lib variables?
    write_csr(0x899, maincall_info + 100);      // DasicsLibBound22
    write_csr(0x89a, maincall_info);            // DasicsLibBound23

    write_csr(0x8a3, (uint64_t)&dasics_maincall);  // DasicsMaincallEntry

    unsigned long dasicsMainCfg = read_csr(0x880);
    printf(main_prompt1, dasicsMainCfg);

    printf(main_prompt2);
    printf(maincall_info);
    //write_csr(0x5c2, 0xffffffff);  // write to dasicsMainBound1, illegalInstr xcpt
    dasics_lib1();
    printf(main_prompt3, read_csr(0xce0));
    printf(main_prompt4, read_csr(0xce1));
    printf(maincall_info);
    sys_exit();
}

void dasics_maincall(void) {
    maincall_info[36] = 'y';
    asm volatile ("ld	   s0, 8(sp)\n"\
                  "addi	   sp, sp, 16\n"\
                  "pulpret x0, 0, x1");
}

// Used to label the lower boundary of user main functions
void dasics_dummy(void) {
    asm volatile ("nop");
}
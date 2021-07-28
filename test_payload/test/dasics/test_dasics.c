#include <stdio.h>
#include "stdcsr.h"
#include "sys/syscall.h"

#define STACK_TOP 0x80503000UL
#define STACK_BOTTOM 0x80501000UL

static char secret[100] = "It's the secret!\n";
static char pub_readonly[100] = "Info: Enter Lib1 ...\n";
static char pub_rwbuffer[100] = "It's public rw buffer!\n";

static void dasics_lib1(void) {
    printf("%s", pub_readonly);           // That's ok
    printf("%s", pub_rwbuffer);           // That's ok
    pub_rwbuffer[10] = pub_readonly[12];  // That's ok
    pub_rwbuffer[12] = 'B';               // That's ok
    pub_rwbuffer[13] = 'B';               // That's ok
    printf("%s", pub_rwbuffer);           // That's ok
    pub_readonly[15] = 0x1;               // raise DasicsStoreAccessFault
    // pub_rwbuffer[0] = secret[0];          // raise DasicsLoadAccessFault
    printf("%s", secret);                 // That's ok, but seems a little weird?

    // unsigned long dasicsMainCfg = read_csr(0x880);  // raise illegalInstr exception
    // write_csr(0x882, 0x0);  // raise illegalInstr exception, too
}

void dasics_main(void) {
    unsigned long dasicsMainCfg = read_csr(0x880);
    printf("DasicsMainCfg: 0x%lx\n", dasicsMainCfg);
    printf("STACK_TOP: 0x%x, STACK_BOTTOM: 0x%x\n", STACK_TOP, STACK_BOTTOM);
    printf("libPrompt base addr: 0x%x\n", pub_readonly);

    write_csr(0x881, 0x080b0a0bUL);  // DasicsLibCfg0, the first config
    write_csr(0x883, STACK_TOP);           // DasicsLibBound0
    write_csr(0x884, STACK_BOTTOM);        // DasicsLibBound1
    write_csr(0x885, pub_readonly + 100);  // DasicsLibBound2
    write_csr(0x886, pub_readonly);        // DasicsLibBound3
    write_csr(0x887, pub_rwbuffer + 100);  // DasicsLibBound4
    write_csr(0x888, pub_rwbuffer);        // DasicsLibBound5
    write_csr(0x889, secret + 100);        // DasicsLibBound6
    write_csr(0x88a, secret);              // DasicsLibBound7

    printf("Info: Ready to enter dasics_lib1.\n");
    //write_csr(0x5c2, 0xffffffff);  // write to dasicsMainBound1, illegalInstr xcpt
    dasics_lib1();
    sys_exit();
}

#include <stdio.h>
#include "stdcsr.h"
#include "sys/syscall.h"

#define STACK_TOP 0x80504000
#define STACK_BOTTOM 0x80500000

static char secret[100] = "It's the secret!\n";
static char pub_readonly[100] = "It's public readonly area!\n";
static char pub_rwbuffer[100] = "It's public rw buffer!\n";

static void dasics_lib1(void) {
    printf("Info: Enter Lib1 ...\n");
    unsigned long dasicsMainCfg = read_csr(0x880);  // raise illegalInstr exception
    printf("[LIB1] DasicsMainCfg: 0x%lx\n", dasicsMainCfg);
    write_csr(0x882, 0x0);  // raise illegalInstr exception, too
}

void dasics_main(void) {
    unsigned long dasicsMainCfg = read_csr(0x880);
    printf("DasicsMainCfg: 0x%lx\n", dasicsMainCfg);
    
    write_csr(0x881, 0xf);  // DasicsLibCfg0, the first config
    write_csr(0x883, STACK_TOP);  // DasicsLibBound0
    write_csr(0x884, STACK_BOTTOM);  // DasicsLibBound1

    printf("Info: Ready to enter dasics_lib1.\n");
    printf("Info: debug\n");
    //write_csr(0x5c2, 0xffffffff);  // write to dasicsMainBound1, illegalInstr xcpt
    dasics_lib1();
    sys_exit();
}

#include <stdio.h>
#include "stdcsr.h"
#include "sys/syscall.h"

void dasics_example(void) {
    unsigned long dasicsMainCfg = read_csr(0x880);
    printf("DasicsMainCfg: 0x%lx\n", dasicsMainCfg);
    sys_exit();
}

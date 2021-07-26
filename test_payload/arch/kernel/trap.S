#include <arch/asm.h>
#include <arch/csr.h>

ENTRY(setup_exception)
  la t0, exception_handler_entry
  csrw CSR_STVEC, t0
  not t0, x0
  csrs CSR_SIE, t0
  jr ra
ENDPROC(setup_exception)

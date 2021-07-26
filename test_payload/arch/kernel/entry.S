#include <arch/asm.h>
#include <arch/csr.h>

.macro SAVE_CONTEXT
  .local _restore_kernel_tpsp
  .local _save_context
  /*
   * If coming from userspace, preserve the user thread pointer and load
   * the kernel thread pointer.  If we came from the kernel, sscratch
   * will contain 0, and we should continue on the current TP.
   */
  csrrw tp, CSR_SSCRATCH, tp
  bnez tp, _save_context

_restore_kernel_tpsp:
  csrr tp, CSR_SSCRATCH
  sd sp, PCB_KERNEL_SP(tp)
_save_context:
  sd sp, PCB_USER_SP(tp)
  ld sp, PCB_KERNEL_SP(tp)
  addi sp, sp, -(OFFSET_SIZE)

  sd x1,  OFFSET_REG_RA(sp)
  sd x3,  OFFSET_REG_GP(sp)
  sd x5,  OFFSET_REG_T0(sp)
  sd x6,  OFFSET_REG_T1(sp)
  sd x7,  OFFSET_REG_T2(sp)
  sd x8,  OFFSET_REG_S0(sp)
  sd x9,  OFFSET_REG_S1(sp)
  sd x10, OFFSET_REG_A0(sp)
  sd x11, OFFSET_REG_A1(sp)
  sd x12, OFFSET_REG_A2(sp)
  sd x13, OFFSET_REG_A3(sp)
  sd x14, OFFSET_REG_A4(sp)
  sd x15, OFFSET_REG_A5(sp)
  sd x16, OFFSET_REG_A6(sp)
  sd x17, OFFSET_REG_A7(sp)
  sd x18, OFFSET_REG_S2(sp)
  sd x19, OFFSET_REG_S3(sp)
  sd x20, OFFSET_REG_S4(sp)
  sd x21, OFFSET_REG_S5(sp)
  sd x22, OFFSET_REG_S6(sp)
  sd x23, OFFSET_REG_S7(sp)
  sd x24, OFFSET_REG_S8(sp)
  sd x25, OFFSET_REG_S9(sp)
  sd x26, OFFSET_REG_S10(sp)
  sd x27, OFFSET_REG_S11(sp)
  sd x28, OFFSET_REG_T3(sp)
  sd x29, OFFSET_REG_T4(sp)
  sd x30, OFFSET_REG_T5(sp)
  sd x31, OFFSET_REG_T6(sp)

  /*
   * Disable user-mode memory access as it should only be set in the
   * actual user copy routines.
   *
   * Disable the FPU to detect illegal usage of floating point in kernel
   * space.
   */
  li t0, SR_SUM | SR_FS

  ld s0, PCB_USER_SP(tp)
  csrrc s1, CSR_SSTATUS, t0
  csrr s2, CSR_SEPC
  csrr s3, CSR_STVAL
  csrr s4, CSR_SCAUSE
  csrr s5, CSR_SSCRATCH
  sd s0,   OFFSET_REG_SP(sp)
  sd s1,   OFFSET_REG_SSTATUS(sp)
  sd s2,   OFFSET_REG_SEPC(sp)
  sd s3,   OFFSET_REG_SBADADDR(sp)
  sd s4,   OFFSET_REG_SCAUSE(sp)
  sd s5,   OFFSET_REG_TP(sp)
.endm

.macro RESTORE_CONTEXT
  ld a0, OFFSET_REG_SSTATUS(sp)
  ld a2, OFFSET_REG_SEPC(sp)
  csrw CSR_SSTATUS, a0
  csrw CSR_SEPC, a2

  ld x1,  OFFSET_REG_RA(sp)
  ld x3,  OFFSET_REG_GP(sp)
  ld x4,  OFFSET_REG_TP(sp)
  ld x5,  OFFSET_REG_T0(sp)
  ld x6,  OFFSET_REG_T1(sp)
  ld x7,  OFFSET_REG_T2(sp)
  ld x8,  OFFSET_REG_S0(sp)
  ld x9,  OFFSET_REG_S1(sp)
  ld x10, OFFSET_REG_A0(sp)
  ld x11, OFFSET_REG_A1(sp)
  ld x12, OFFSET_REG_A2(sp)
  ld x13, OFFSET_REG_A3(sp)
  ld x14, OFFSET_REG_A4(sp)
  ld x15, OFFSET_REG_A5(sp)
  ld x16, OFFSET_REG_A6(sp)
  ld x17, OFFSET_REG_A7(sp)
  ld x18, OFFSET_REG_S2(sp)
  ld x19, OFFSET_REG_S3(sp)
  ld x20, OFFSET_REG_S4(sp)
  ld x21, OFFSET_REG_S5(sp)
  ld x22, OFFSET_REG_S6(sp)
  ld x23, OFFSET_REG_S7(sp)
  ld x24, OFFSET_REG_S8(sp)
  ld x25, OFFSET_REG_S9(sp)
  ld x26, OFFSET_REG_S10(sp)
  ld x27, OFFSET_REG_S11(sp)
  ld x28, OFFSET_REG_T3(sp)
  ld x29, OFFSET_REG_T4(sp)
  ld x30, OFFSET_REG_T5(sp)
  ld x31, OFFSET_REG_T6(sp)

  ld x2,  OFFSET_REG_SP(sp)
.endm

ENTRY(enable_preempt)
  ld t1, current_running
  ld t0, PCB_PREEMPT_COUNT(t1)
  beq t0, zero, do_enable
	addi t0, t0, -1
	sd t0, PCB_PREEMPT_COUNT(t1)
  beq t0, zero, do_enable
  jr ra
do_enable:
  not t0, x0
  csrs CSR_SIE, t0
  jr ra
ENDPROC(enable_preempt)

ENTRY(disable_preempt)
  csrw CSR_SIE, zero
  ld t1, current_running
  ld t0, PCB_PREEMPT_COUNT(t1)
  addi t0, t0, 1
  sd t0, PCB_PREEMPT_COUNT(t1)
  jr ra
ENDPROC(disable_preempt)

ENTRY(enable_interrupt)
  li t0, SR_SIE
  csrs CSR_SSTATUS, t0
  jr ra
ENDPROC(enable_interrupt)

ENTRY(disable_interrupt)
  li t0, SR_SIE
  csrs CSR_SSTATUS, t0
  jr ra
ENDPROC(disable_interrupt)

// the address of previous pcb in a0
// the address of next pcb in a1
ENTRY(switch_to)
  // save all callee save registers on kernel stack
  addi sp, sp, -(SWITCH_TO_SIZE)
  sd ra,  SWITCH_TO_RA(sp)
  sd s0,  SWITCH_TO_S0(sp)
  sd s1,  SWITCH_TO_S1(sp)
  sd s2,  SWITCH_TO_S2(sp)
  sd s3,  SWITCH_TO_S3(sp)
  sd s4,  SWITCH_TO_S4(sp)
  sd s5,  SWITCH_TO_S5(sp)
  sd s6,  SWITCH_TO_S6(sp)
  sd s7,  SWITCH_TO_S7(sp)
  sd s8,  SWITCH_TO_S8(sp)
  sd s9,  SWITCH_TO_S9(sp)
  sd s10, SWITCH_TO_S10(sp)
  sd s11, SWITCH_TO_S11(sp)
  sd sp,  SWITCH_TO_SP(sp)
  sd sp,  PCB_KERNEL_SP(a0)

  // restore next
  add tp, a1, zero
  ld sp,  PCB_KERNEL_SP(tp)
  ld ra,  SWITCH_TO_RA(sp)
  ld s0,  SWITCH_TO_S0(sp)
  ld s1,  SWITCH_TO_S1(sp)
  ld s2,  SWITCH_TO_S2(sp)
  ld s3,  SWITCH_TO_S3(sp)
  ld s4,  SWITCH_TO_S4(sp)
  ld s5,  SWITCH_TO_S5(sp)
  ld s6,  SWITCH_TO_S6(sp)
  ld s7,  SWITCH_TO_S7(sp)
  ld s8,  SWITCH_TO_S8(sp)
  ld s9,  SWITCH_TO_S9(sp)
  ld s10, SWITCH_TO_S10(sp)
  ld s11, SWITCH_TO_S11(sp)
  ld sp,  SWITCH_TO_SP(sp)
  addi sp, sp, SWITCH_TO_SIZE
  jr ra
ENDPROC(switch_to)

ENTRY(ret_from_exception)
  /* check if we return to kernel or a normal thread */
  la t0, pid0_pcb
  beq tp, t0, restore

  /* return to a normal thread(not the pid0 thread) now */
  addi t0, sp, OFFSET_SIZE
  sd t0, PCB_KERNEL_SP(tp)
  csrw CSR_SSCRATCH, tp


restore:
  RESTORE_CONTEXT
  sret
ENDPROC(ret_from_exception)

ENTRY(exception_handler_entry)
  SAVE_CONTEXT

  csrw CSR_SSCRATCH, x0

  /* Load the global pointer */
  .option push
  .option norelax
  la gp, __global_pointer$
  .option pop

  la ra, ret_from_exception

  move a0,sp
  move a1,s3
  move a2,s4

  la t0, interrupt_helper
  jr t0
ENDPROC(exception_handler_entry)

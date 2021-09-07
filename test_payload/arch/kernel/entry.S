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

  /* Save N Extension CSRs */
  csrr s0, CSR_USTATUS
  sd   s0, OFFSET_REG_USTATUS(sp)
  csrr s0, CSR_UEPC
  sd   s0, OFFSET_REG_UEPC(sp)
  csrr s0, CSR_UTVAL
  sd   s0, OFFSET_REG_UBADADDR(sp)
  csrr s0, CSR_UCAUSE
  sd   s0, OFFSET_REG_UCAUSE(sp)
  csrr s0, CSR_UTVEC
  sd   s0, OFFSET_REG_UTVEC(sp)
  csrr s0, CSR_UIE
  sd   s0, OFFSET_REG_UIE(sp)
  csrr s0, CSR_UIP
  sd   s0, OFFSET_REG_UIP(sp)
  csrr s0, CSR_USCRATCH
  sd   s0, OFFSET_REG_USCRATCH(sp)

  /* Save DASICS context */
  csrr s0, CSR_DUMCFG
  sd   s0, OFFSET_REG_DUMCFG(sp)
  csrr s0, CSR_DUMBOUNDHI
  sd   s0, OFFSET_REG_DUMBOUNDHI(sp)
  csrr s0, CSR_DUMBOUNDLO
  sd   s0, OFFSET_REG_DUMBOUNDLO(sp)

  csrr s0, CSR_DLCFG0
  sd   s0, OFFSET_REG_DLCFG0(sp)
  csrr s0, CSR_DLCFG1
  sd   s0, OFFSET_REG_DLCFG1(sp)

  csrr s0, CSR_DLBOUND0
  sd   s0, OFFSET_REG_DLBOUND0(sp)
  csrr s0, CSR_DLBOUND1
  sd   s0, OFFSET_REG_DLBOUND1(sp)
  csrr s0, CSR_DLBOUND2
  sd   s0, OFFSET_REG_DLBOUND2(sp)
  csrr s0, CSR_DLBOUND3
  sd   s0, OFFSET_REG_DLBOUND3(sp)
  csrr s0, CSR_DLBOUND4
  sd   s0, OFFSET_REG_DLBOUND4(sp)
  csrr s0, CSR_DLBOUND5
  sd   s0, OFFSET_REG_DLBOUND5(sp)
  csrr s0, CSR_DLBOUND6
  sd   s0, OFFSET_REG_DLBOUND6(sp)
  csrr s0, CSR_DLBOUND7
  sd   s0, OFFSET_REG_DLBOUND7(sp)
  csrr s0, CSR_DLBOUND8
  sd   s0, OFFSET_REG_DLBOUND8(sp)
  csrr s0, CSR_DLBOUND9
  sd   s0, OFFSET_REG_DLBOUND9(sp)
  csrr s0, CSR_DLBOUND10
  sd   s0, OFFSET_REG_DLBOUND10(sp)
  csrr s0, CSR_DLBOUND11
  sd   s0, OFFSET_REG_DLBOUND11(sp)
  csrr s0, CSR_DLBOUND12
  sd   s0, OFFSET_REG_DLBOUND12(sp)
  csrr s0, CSR_DLBOUND13
  sd   s0, OFFSET_REG_DLBOUND13(sp)
  csrr s0, CSR_DLBOUND14
  sd   s0, OFFSET_REG_DLBOUND14(sp)
  csrr s0, CSR_DLBOUND15
  sd   s0, OFFSET_REG_DLBOUND15(sp)
  csrr s0, CSR_DLBOUND16
  sd   s0, OFFSET_REG_DLBOUND16(sp)
  csrr s0, CSR_DLBOUND17
  sd   s0, OFFSET_REG_DLBOUND17(sp)
  csrr s0, CSR_DLBOUND18
  sd   s0, OFFSET_REG_DLBOUND18(sp)
  csrr s0, CSR_DLBOUND19
  sd   s0, OFFSET_REG_DLBOUND19(sp)
  csrr s0, CSR_DLBOUND20
  sd   s0, OFFSET_REG_DLBOUND20(sp)
  csrr s0, CSR_DLBOUND21
  sd   s0, OFFSET_REG_DLBOUND21(sp)
  csrr s0, CSR_DLBOUND22
  sd   s0, OFFSET_REG_DLBOUND22(sp)
  csrr s0, CSR_DLBOUND23
  sd   s0, OFFSET_REG_DLBOUND23(sp)
  csrr s0, CSR_DLBOUND24
  sd   s0, OFFSET_REG_DLBOUND24(sp)
  csrr s0, CSR_DLBOUND25
  sd   s0, OFFSET_REG_DLBOUND25(sp)
  csrr s0, CSR_DLBOUND26
  sd   s0, OFFSET_REG_DLBOUND26(sp)
  csrr s0, CSR_DLBOUND27
  sd   s0, OFFSET_REG_DLBOUND27(sp)
  csrr s0, CSR_DLBOUND28
  sd   s0, OFFSET_REG_DLBOUND28(sp)
  csrr s0, CSR_DLBOUND29
  sd   s0, OFFSET_REG_DLBOUND29(sp)
  csrr s0, CSR_DLBOUND30
  sd   s0, OFFSET_REG_DLBOUND30(sp)
  csrr s0, CSR_DLBOUND31
  sd   s0, OFFSET_REG_DLBOUND31(sp)

  csrr s0, CSR_DMAINCALL
  sd   s0, OFFSET_REG_DMAINCALL(sp)
  csrr s0, CSR_DRETURNPC
  sd   s0, OFFSET_REG_DRETURNPC(sp)
  csrr s0, CSR_DFZRETURN
  sd   s0, OFFSET_REG_DFZRETURN(sp)
.endm

.macro RESTORE_CONTEXT
  /* Restore Sstatus and Sepc */
  ld a0, OFFSET_REG_SSTATUS(sp)
  ld a2, OFFSET_REG_SEPC(sp)
  csrw CSR_SSTATUS, a0
  csrw CSR_SEPC, a2

  /* Restore N Extension CSRs */
  ld   s0, OFFSET_REG_USTATUS(sp)
  csrw CSR_USTATUS, s0
  ld   s0, OFFSET_REG_UEPC(sp)
  csrw CSR_UEPC, s0
  ld   s0, OFFSET_REG_UBADADDR(sp)
  csrw CSR_UTVAL, s0
  ld   s0, OFFSET_REG_UCAUSE(sp)
  csrw CSR_UCAUSE, s0
  ld   s0, OFFSET_REG_UTVEC(sp)
  csrw CSR_UTVEC, s0
  ld   s0, OFFSET_REG_UIE(sp)
  csrw CSR_UIE, s0
  ld   s0, OFFSET_REG_UIP(sp)
  csrw CSR_UIP, s0
  ld   s0, OFFSET_REG_USCRATCH(sp)
  csrw CSR_USCRATCH, s0

  /* Restore DASICS context */
  ld   s0, OFFSET_REG_DUMCFG(sp)
  ori  s0, s0, 0x1
  csrw CSR_DUMCFG, s0
  ld   s0, OFFSET_REG_DUMBOUNDHI(sp)
  csrw CSR_DUMBOUNDHI, s0
  ld   s0, OFFSET_REG_DUMBOUNDLO(sp)
  csrw CSR_DUMBOUNDLO, s0

  ld   s0, OFFSET_REG_DLCFG0(sp)
  csrw CSR_DLCFG0, s0
  ld   s0, OFFSET_REG_DLCFG1(sp)
  csrw CSR_DLCFG1, s0

  ld   s0, OFFSET_REG_DLBOUND0(sp)
  csrw CSR_DLBOUND0, s0
  ld   s0, OFFSET_REG_DLBOUND1(sp)
  csrw CSR_DLBOUND1, s0
  ld   s0, OFFSET_REG_DLBOUND2(sp)
  csrw CSR_DLBOUND2, s0
  ld   s0, OFFSET_REG_DLBOUND3(sp)
  csrw CSR_DLBOUND3, s0
  ld   s0, OFFSET_REG_DLBOUND4(sp)
  csrw CSR_DLBOUND4, s0
  ld   s0, OFFSET_REG_DLBOUND5(sp)
  csrw CSR_DLBOUND5, s0
  ld   s0, OFFSET_REG_DLBOUND6(sp)
  csrw CSR_DLBOUND6, s0
  ld   s0, OFFSET_REG_DLBOUND7(sp)
  csrw CSR_DLBOUND7, s0
  ld   s0, OFFSET_REG_DLBOUND8(sp)
  csrw CSR_DLBOUND8, s0
  ld   s0, OFFSET_REG_DLBOUND9(sp)
  csrw CSR_DLBOUND9, s0
  ld   s0, OFFSET_REG_DLBOUND10(sp)
  csrw CSR_DLBOUND10, s0
  ld   s0, OFFSET_REG_DLBOUND11(sp)
  csrw CSR_DLBOUND11, s0
  ld   s0, OFFSET_REG_DLBOUND12(sp)
  csrw CSR_DLBOUND12, s0
  ld   s0, OFFSET_REG_DLBOUND13(sp)
  csrw CSR_DLBOUND13, s0
  ld   s0, OFFSET_REG_DLBOUND14(sp)
  csrw CSR_DLBOUND14, s0
  ld   s0, OFFSET_REG_DLBOUND15(sp)
  csrw CSR_DLBOUND15, s0
  ld   s0, OFFSET_REG_DLBOUND16(sp)
  csrw CSR_DLBOUND16, s0
  ld   s0, OFFSET_REG_DLBOUND17(sp)
  csrw CSR_DLBOUND17, s0
  ld   s0, OFFSET_REG_DLBOUND18(sp)
  csrw CSR_DLBOUND18, s0
  ld   s0, OFFSET_REG_DLBOUND19(sp)
  csrw CSR_DLBOUND19, s0
  ld   s0, OFFSET_REG_DLBOUND20(sp)
  csrw CSR_DLBOUND20, s0
  ld   s0, OFFSET_REG_DLBOUND21(sp)
  csrw CSR_DLBOUND21, s0
  ld   s0, OFFSET_REG_DLBOUND22(sp)
  csrw CSR_DLBOUND22, s0
  ld   s0, OFFSET_REG_DLBOUND23(sp)
  csrw CSR_DLBOUND23, s0
  ld   s0, OFFSET_REG_DLBOUND24(sp)
  csrw CSR_DLBOUND24, s0
  ld   s0, OFFSET_REG_DLBOUND25(sp)
  csrw CSR_DLBOUND25, s0
  ld   s0, OFFSET_REG_DLBOUND26(sp)
  csrw CSR_DLBOUND26, s0
  ld   s0, OFFSET_REG_DLBOUND27(sp)
  csrw CSR_DLBOUND27, s0
  ld   s0, OFFSET_REG_DLBOUND28(sp)
  csrw CSR_DLBOUND28, s0
  ld   s0, OFFSET_REG_DLBOUND29(sp)
  csrw CSR_DLBOUND29, s0
  ld   s0, OFFSET_REG_DLBOUND30(sp)
  csrw CSR_DLBOUND30, s0
  ld   s0, OFFSET_REG_DLBOUND31(sp)
  csrw CSR_DLBOUND31, s0

  ld   s0, OFFSET_REG_DMAINCALL(sp)
  csrw CSR_DMAINCALL, s0
  ld   s0, OFFSET_REG_DRETURNPC(sp)
  csrw CSR_DRETURNPC, s0
  ld   s0, OFFSET_REG_DFZRETURN(sp)
  csrw CSR_DFZRETURN, s0

  /* Restore general registers */
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

  /* Set DASICS configuration for S-Mode */
  csrr t0, CSR_DUMCFG
  ori  t0, t0, 0x1
  csrw CSR_DUMCFG, t0

  li   t0, 0x0b0b0c0aUL
  csrw CSR_DLCFG0, t0
  csrw CSR_DLCFG1, x0

  la   t0, __RODATA_END__
  la   t1, __RODATA_BEGIN__
  csrw CSR_DLBOUND0, t0
  csrw CSR_DLBOUND1, t1

  la   t0, __SFREEZONE_TEXT_END__
  la   t1, __SFREEZONE_TEXT_BEGIN__
  addi t0, t0, -2
  csrw CSR_DLBOUND2, t0
  csrw CSR_DLBOUND3, t1

  la   t0, __SFREEZONE_DATA_END__
  la   t1, __SFREEZONE_DATA_BEGIN__
  csrw CSR_DLBOUND4, t0
  csrw CSR_DLBOUND5, t1

  ld   t0, PCB_USER_STACK_BASE(tp)    // FIXME: if kmalloc is called ?
  ld   t1, PCB_KERNEL_STACK_BASE(tp)
  li   t2, 4096  // PAGE_SIZE
  add  t0, t0, t2
  csrw CSR_DLBOUND6, t0
  csrw CSR_DLBOUND7, t1

  la   t0, dasics_smaincall
  csrw CSR_DMAINCALL, t0

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

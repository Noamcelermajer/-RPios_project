#include "arm/sysregs.h"
#include "mm.h"
.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0,#0xFF		// Check processor id
	cbz	x0, master			// Hang for all non-primary CPU
	b	proc_hang

proc_hang: 
	b 	proc_hang

master:

    // set stack before our code
    ldr     x1, =_start

    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    msr     sp_el1, x1

    /* qemu starts at EL2, than is not possible to access any register of EL3 */
	//ldr	x0, =SCR_VALUE
	//msr	scr_el3, x0


    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr

    // enable AArch64 in EL1
    ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0
    
    // Setup SCTLR access
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0

    // set up exception handlers
    ldr     x2, =_vectors
    msr     vbar_el1, x2
    
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    
    adr	x0, el1_entry		
	msr	elr_el2, x0

    eret

el1_entry:
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero

	mov	sp, #LOW_MEMORY
	bl	kernel_main
	b 	proc_hang	


    .align 11
_vectors:
    // synchronous
    .align  7
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler

    // IRQ
    .align  7
    mov     x0, #1
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler

    // FIQ
    .align  7
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler

    // SError
    .align  7
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler
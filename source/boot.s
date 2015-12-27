// To keep this in the first portion of the binary.
.section .text.boot

// Make _start global.
.globl _start

// Entry point for the kernel.
// r15 -> should begin execution at 0x8000.
// r0 -> 0x00000000
// r1 -> 0x00000C42
// r2 -> 0x00000100 - start of ATAGS
// preserve these registers as argument for kmain
_start:
	// Setup the stack.
	mov sp, #0x2000000
    push {ip, lr}
    
	// Clear out bss.
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b       2f
    
1:
	// store multiple at r4.
	stmia r4!, {r5-r8}
    
	// If we are still below bss_end, loop.
2:
	cmp r4, r9
	blo 1b
    
    push {r0-r2}
    
    // Setup access to the OK LED
    ldr r0, =0x3F200000
    mov r1, #1
    lsl r1, #21
    str r1, [r0,#0x10]
    //mov r1, #1
    //lsl r1, #15
    //str r1, [r0,#0x20]
    
    // Call static constructors
    ldr r3, =_init_array
    blx r3
    
    pop {r0-r2}
    
	// Call kmain
	ldr r3, =kmain
	blx r3
    
	// halt
1:
	wfe
	b 1b

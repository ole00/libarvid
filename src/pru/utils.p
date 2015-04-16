.setcallreg r26.w2
.origin 0
.entrypoint Start


// ************************************************
// Header file for PRU ASM programs
// ************************************************

// ************************************************
// NOP - no operation
// ************************************************
.macro NOP
	mov r16, r16
.endm

// ************************************************
// SAVE_RETURN_ADDRESS
// by default stores to r26.w0
// ************************************************
.macro SAVE_RETURN_ADDRESS
.mparam save_reg=r26.w0
	mov save_reg, r26.w2
.endm


// ************************************************
// RESTORE_RETURN_ADDRESS
// ************************************************
.macro RESTORE_RETURN_ADDRESS
.mparam rest_reg=r26.w0
	mov r26.w2, rest_reg
.endm

// ************************************************
// RETURN - returns (jumps) to saved address
// ************************************************
.macro RETURN
.mparam save_reg=r26.w0
	jmp save_reg
.endm


// ************************************************
// GOTO - goto label
// ************************************************
.macro GOTO
.mparam label
	QBA label
.endm


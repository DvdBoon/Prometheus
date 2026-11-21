| cloud test build only: asm entry points with the register ABI
	.text
	.globl	SetMemoryMode, NoOp, HardInterruptCode
SetMemoryMode:
	moveml	%d0-%d1/%a0-%a1,%sp@-
	movel	%d7,%sp@-
	movel	%a0,%sp@-
	jsr	SetMemoryModeC
	addql	#8,%sp
	moveml	%sp@+,%d0-%d1/%a0-%a1
	rts
HardInterruptCode:
	movel	%a1,%sp@-
	jsr	HardInterruptCodeC
	addql	#4,%sp
	tstl	%d0
	rts
NoOp:
	rts

| 3DLabsPermedia2.chip - asm entry points (amiga-gcc, register ABI)
| Must be the first object in the link: a library run from the shell
| returns at once.
	.text
	.globl	_ReturnError
	.globl	_SetMemoryMode
	.globl	_HardInterruptCode
	.globl	_NoOp

_ReturnError:
	moveq	#-1,%d0
	rts

| bi->SetMemoryMode: rtg.library expects d0/d1/a0/a1 to be preserved
_SetMemoryMode:
	moveml	%d0-%d1/%a0-%a1,%sp@-
	jsr	_SetMemoryModeC		| a0 = bi, d7 = format
	moveml	%sp@+,%d0-%d1/%a0-%a1
	rts

| bi->HardInterrupt.is_Code: a1 = is_Data = bi, Z flag must match d0
_HardInterruptCode:
	jsr	_HardInterruptCodeC
	tstl	%d0
	rts

| bi->SetWriteMask / SetClearMask / SetReadPlane: nothing to do
_NoOp:
	rts

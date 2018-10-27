#include <machine/asm.h>
#include "i8042.h"

.intel_syntax

.global _C_LABEL(kbc_asm_ih)

	.text
_C_LABEL(kbc_asm_ih):
		mov ecx, 5

L1:		cmp ecx, 0
		jb L4
		xor eax, eax

		in eax, STAT_REG
		
		and eax, OBF
		cmp eax, 0x0001				
		je L2
		dec ecx
		jmp L1

L2:		mov ebx, eax
		xor eax, eax
		in eax, OUT_BUF
		mov _C_LABEL(stat), ebx
		
		mov edx, PAR_ERR
		or edx, TO_ERR
		and ebx, edx

		cmp ebx, 0
		je L3
		dec ecx
		jmp L1

L3:		mov _C_LABEL(status), al
		ret

L4:		mov BYTE PTR _C_LABEL(status), -1
		ret

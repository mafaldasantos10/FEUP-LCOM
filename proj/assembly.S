#include <machine/asm.h>
#include "i8042.h"

.intel_syntax

.global _C_LABEL(VSync)

.text
_C_LABEL(VSync):

    PUSH	AX
		PUSH	DX

		MOV	DX, INPUT_STATUS	# VGA Input Status Register
						              #
L1:				                #
		IN	AL, DX	          # Port 03DAH
		TEST	AL, VRETRACE		# checks bit 3
		JZ	L3		            # No, continue waiting
						              #
L2:			    	            #
		IN	AL, DX		        # Port 03DAH
		TEST	AL, VRETRACE		# checks bit 3
		JNZ	L2		            # No, continue waiting

    POP	DX
		POP	AX

L3:	RET

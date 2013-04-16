
.ifdef __dsPIC30F
        .include "p30fxxxx.inc"
.endif
.ifdef __dsPIC33F
        .include "p33Fxxxx.inc"
.endif

        .global  _AverageADC128

         .text
_AverageADC128:
		mov		W0, W4		; move source address
		CLR		A		; 0 40 bit Acc

ACC_S:	REPEAT	#127			; add em all up
		ADD		[W4++], #4, A ; signed 16 add to ACCA (right shift 4 bits)
	                                      ; The data that we want is now stored in the 15 LSB of ACCAH and the 1 MSB of ACCAL
	                                      ; If we shift the data left one bit and call SAC.R the data will be bashed because
	                                      ; The accumulator will be signed.  There for we must work around this little problem 
		SAC		A, W0	      ; Move ACCAH to W0
		SL		W0, #1, W0    ; Shift W0 left by one bit.  
 		SFTAC           A, #-1        ; Shift Accumulator left by one bit.
	        SAC.R           A, W1         ; Move ACCAH to W1
		AND             #0x0001, W1   ; W1 &= 0x0001
	        IOR             W0, W1, W0    ; WO = WO | W1
		return
;--------End of All Code Sections ---------------------------------------------
        .end                               ;End of program code in this file




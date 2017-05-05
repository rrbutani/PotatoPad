; Print.s
; Student names: Junlin Zhu and Rahul Butani
; Last modification date: 03/21/17
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB


;-----------------------LCD_OutDec-----------------------
; Recursively output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; Symbolic binding
n	EQU		0
LCD_OutDec
		PUSH	{R11,LR}
		CMP		R0, #10
		BLO		FIN
		SUB		SP, #8 		; ALLOCATE MEMORY
		MOV		R11, SP		; FRAME POINTER
		MOV		R1, #10
		UDIV	R2, R0, R1
		MUL		R1, R2
		SUB		R0, R1
		STR		R0, [R11, #n]
		MOV		R0, R2
		BL		LCD_OutDec
		LDR		R0, [R11, #n]
		ADD		SP, #8		; DEALLOCATE MEMORY
FIN		ADD		R0, #'0'
		BL		ST7735_OutChar
		MOV		R0, R4
		POP		{R11,PC}
;* * * * * * * * End of LCD_OutDec * * * * * * * *


; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
ERROR   DCB  "*.***",0,0,0	; EXTRA 0 FOR ALIGNMENT
; Symbolic binding
OUTPUT	EQU		0
LCD_OutFix
		PUSH	{R4, LR}
		SUB		SP, #8	; ALLOCATE MEMORY
		MOV		R1, #9999
		CMP		R0, R1
		BLS		VALID
		LDR		R0, =ERROR
		B		PRINT
VALID	MOV		R1, #4
		MOV		R2, #10
LOOP	UDIV	R3, R0, R2
		MUL		R4, R3, R2
		SUB		R4, R0, R4
		ADD		R4,	#'0'
		STRB	R4, [SP, R1]
		MOV		R0, R3
		SUBS	R1, #1
		BNE		LOOP
		STRB	R4, [SP, #OUTPUT]
		MOV		R0, #'.'
		STRB	R0, [SP, #1]
		MOV		R0, #0
		STRB	R0, [SP, #5]
		ADD		R0, SP, #OUTPUT
PRINT	BL		ST7735_OutString
		ADD		SP, #8	; DEALLOCATE MEMORY
		POP		{R4, PC}
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file

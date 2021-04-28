.ORIG x3000
LEA R1, total
LDW R2, R1, #0
LEA R1, start
LDW R3, R1, #0
AND R0, R0, #0


Loop	 LDB R4, R3, #0
	 ADD R0, R0, R4
	 ADD R3, R3, #1
	 ADD R2, R2, #-1
	 BRp Loop

STW  R0, R3, #0
JSRR R0

total .FILL #20
start .FILL xC000
.END 

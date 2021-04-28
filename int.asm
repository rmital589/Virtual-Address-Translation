	 .ORIG x1200
	 ADD R6, R6, #-2
	 STW R0, R6, #0
	 ADD R6, R6, #-2
	 STW R1, R6, #0	 
	 ADD R6, R6, #-2
	 STW R2, R6, #0
	 AND R0, R0, #0
	 AND R1, R1, #0
	 AND R2, R2, #0
	 LEA R0, table 
	 LDW R1, R0, #0
	 LEA R0, pages 
	 LDW R2, R0, #0
Loop LDW R0, R1, #0
	 AND R0, R0, #30
     STW R0, R1, #0
	 ADD R1, R1, #2
	 ADD R2, R2, #-1
	 BRp Loop
	 AND R0, R0, #0
	 AND R1, R1, #0
	 AND R2, R2, #0
	 LDW R2, R6, #0
	 ADD R6, R6, #2
	 LDW R1, R6, #0
	 ADD R6, R6, #2
	 LDW R0, R6, #0
	 ADD R6, R6, #2
	 RTI	
table x1000
pages x0080
.END
/*
    Name 1: Rohan Mital
    UTEID 1: rm53327
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycles_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
	LD_SSP,	
	GATE_PSR,
	PUSH_SSP,
	LD_PSR,
	GATE_INTV,
	DEC_R6,
	LD_R6,
	INC_R6,
	GATE_PTBR,
	GATE_VA,
	LD_VA,
	GATE_MAR,
	LD_PTE,
	GATE_PTE,
	LD_PFN,
	GATE_PFN,
	PFNSHF,
	VASHF,	
	GATE_PA,
/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] <<2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return(x[DRMUX]); }
int GetSR1MUX(int *x)        { return(x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_SSP(int *x)      {return(x[LD_SSP]); }
int GetLD_PSR(int *x)      {return(x[LD_PSR]); }
int GetPUSH_SSP(int *x)       {return(x[PUSH_SSP]); }
int GetGATE_PSR(int *x)    {return(x[GATE_PSR]); }
int GetGATE_INTV(int *x)   {return(x[GATE_INTV]); }
int GetLD_R6(int *x)          {return(x[LD_R6]); }
int GetINC_R6(int *x)         {return(x[INC_R6]); }
int GetDEC_R6(int *x)         {return(x[DEC_R6]); }
int GetGATE_PTBR(int *x)         {return(x[GATE_PTBR]); }
int GetGATE_VA(int *x)         {return(x[GATE_VA]); }
int GetLD_VA(int *x)         {return(x[LD_VA]); }
int GetGATE_MAR(int *x)         {return(x[GATE_MAR]); }
int GetLD_PTE(int *x)         {return(x[LD_PTE]); }
int GetGATE_PTE(int *x)         {return(x[GATE_PTE]); }
int GetLD_PFN(int *x)         {return(x[LD_PFN]); }
int GetGATE_PFN(int *x)         {return(x[GATE_PFN]); }
int GetPFNSHF(int *x)         {return(x[PFNSHF]); }
int GetVASHF(int *x)         {return(x[VASHF]); }
int GetGATE_PA(int *x)         {return(x[GATE_PA]); }
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */
int timer_interrupt; /*timer interrupt flag */
int protect_exception = 0; //protect exception flag
int unaligned_exception = 0; //unaligned mem access flag 
int unknown_exception = 0; //unknown opcode flag 
int translation_done = 0; //translation flag
int page_fault = 0;
typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer = 0x3000*/
/* MODIFY: You may add system latches that are required by your implementation */
int PSR; /*Process Status Register*/
int TEMP_R6; /* Store last Value of R6 */
// int USR initalize to 0xFE00 and in user mode r6 should represent this value 
/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
int PTE;
int PFN;
int PA;
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

   eval_micro_sequencer();   
  cycles_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
  if (CYCLE_COUNT == 300) 
	timer_interrupt = 1;
}

//***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
	CURRENT_LATCHES.PSR = 0x8000;
	CURRENT_LATCHES.INTV = 0;
	CURRENT_LATCHES.TEMP_R6 = 0;
	CURRENT_LATCHES.PTBR = 0x1000;
/* MODIFY: you can add more initialization code HERE */

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/


int signex(int number, int f, int t){
   if (number>>(f-1) == 1) {
      int bshift = 1<<f;
      int num = t - f;
      while (num-- > 0) {
         number += bshift;
         bshift = bshift << 1;
      }
   }
   return number;
}

int latch_to_bus;
int from_address_adder;
int store_state;
int reference, modified, pfn;
void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */
    
    int ir = CURRENT_LATCHES.IR;
    int opcode_6bits = ir >> 12 ; 
    int ir11 = (ir >> 11 ) - (ir >> 12 << 1);
	int current_psr = CURRENT_LATCHES.PSR;
	int current_priority = (current_psr >>9) - (current_psr >> 12 << 3);//PSR[10:8]
	int current_mode = current_psr >> 15; //user or supervisor mode 
	int mar0 = (CURRENT_LATCHES.MAR - CURRENT_LATCHES.MAR>>1<<1);
    
    int *microinstruction = CURRENT_LATCHES.MICROINSTRUCTION;	
	

	if((timer_interrupt == 1)  && (CURRENT_LATCHES.STATE_NUMBER == 18 || CURRENT_LATCHES.STATE_NUMBER == 19))
	{
		
		NEXT_LATCHES.INTV = 0x01;
		NEXT_LATCHES.STATE_NUMBER = 36;
		 
	}

	else if(translation_done == 0 && (CURRENT_LATCHES.STATE_NUMBER == 18 || CURRENT_LATCHES.STATE_NUMBER == 19 || CURRENT_LATCHES.STATE_NUMBER == 15 || CURRENT_LATCHES.STATE_NUMBER == 2 || CURRENT_LATCHES.STATE_NUMBER == 3 || CURRENT_LATCHES.STATE_NUMBER == 6 || CURRENT_LATCHES.STATE_NUMBER == 7 ))

	{
		translation_done = 1;
		NEXT_LATCHES.STATE_NUMBER = 34;//first state of translation
		store_state = CURRENT_LATCHES.STATE_NUMBER;
	}	
	
  else{
    if(GetIRD(microinstruction))
        NEXT_LATCHES.STATE_NUMBER = opcode_6bits;
    else {
            if(GetCOND(microinstruction) == 0)
            NEXT_LATCHES.STATE_NUMBER = GetJ(microinstruction);
            if(GetCOND(microinstruction) == 1)
	{	
            NEXT_LATCHES.STATE_NUMBER = GetJ(microinstruction) | (CURRENT_LATCHES.READY << 1);
	}
            if(GetCOND(microinstruction) == 2)
            NEXT_LATCHES.STATE_NUMBER = GetJ(microinstruction) | (CURRENT_LATCHES.BEN << 2);
            if(GetCOND(microinstruction) == 3)
            NEXT_LATCHES.STATE_NUMBER = GetJ(microinstruction) | ir11;
			if(GetCOND(microinstruction) == 4)
            {
				if(store_state == 18 || store_state == 19)
				NEXT_LATCHES.STATE_NUMBER = 33;
				if(store_state == 15)
				NEXT_LATCHES.STATE_NUMBER = 28;
				if(store_state == 2)
				NEXT_LATCHES.STATE_NUMBER = 29;
				if(store_state == 3)
				NEXT_LATCHES.STATE_NUMBER = 24;
				if(store_state == 6)
				NEXT_LATCHES.STATE_NUMBER = 25;
				if(store_state == 7)
				NEXT_LATCHES.STATE_NUMBER = 23;
                                 
			}
            	    
	
    }    

  }
 
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
     
    
}

int from_memory;
void cycles_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
	int *microinstruction = CURRENT_LATCHES.MICROINSTRUCTION;
	int mar0 = (CURRENT_LATCHES.MAR - CURRENT_LATCHES.MAR>>1<<1); //MAR[0]
	int WE0,WE1;
	static int cycles_mem = 1;
	NEXT_LATCHES.READY = 0;
	
	
	if(GetMIO_EN(microinstruction))
	{
		if( cycles_mem == MEM_CYCLES - 1)
			NEXT_LATCHES.READY = 1;
			
			if(cycles_mem < MEM_CYCLES )
		{
			cycles_mem = cycles_mem + 1;
			
		}	
		
	
		
		else {	
				cycles_mem = 1;
				WE0 = 0;
				WE1 = 0;
		if(GetR_W(microinstruction))
		{
			if (GetDATA_SIZE(microinstruction) == 0 && mar0 == 0)
               WE0 = 1;
            else if (GetDATA_SIZE(microinstruction) == 0 && mar0 == 1)
               WE1 = 1;
            else if (GetDATA_SIZE(microinstruction) == 1 && mar0 == 0) {
               WE1 = 1;
               WE0 = 1;
		}
			if (WE0)
               MEMORY[CURRENT_LATCHES.MAR/2][0] = CURRENT_LATCHES.MDR - (CURRENT_LATCHES.MDR >> 8 << 8);
            if (WE1)
               MEMORY[CURRENT_LATCHES.MAR/2][1] = CURRENT_LATCHES.MDR >> 8;
		}
		else
		{
			from_memory = MEMORY[(CURRENT_LATCHES.MAR>>1<<1)/2][0] + (MEMORY[(CURRENT_LATCHES.MAR>>1<<1)/2][1] << 8);
		}
	}
	
	}
	
	
	
	
}
	int DR;
void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    

latch_to_bus = 0; 
int *microinstruction = CURRENT_LATCHES.MICROINSTRUCTION;
int ir = CURRENT_LATCHES.IR;
int mar0 = CURRENT_LATCHES.MAR - (CURRENT_LATCHES.MAR>>1<<1); //MAR[0]
int amount4 = ir - (ir>>4<<4);
int a = (ir>>5) - (ir>>6<<1); /*arthematic shift */
int d = (ir>>4) - (ir>>5<<1);
int r6 = CURRENT_LATCHES.REGS[6];

int from_lshft1;
int from_addr1mux;
int from_sr1out;
int from_sr2out = CURRENT_LATCHES.REGS[ir - (ir>>3<<3)];;
int SR2mux = (ir>>5)-(ir>>6<<1);
int from_SR2mux;
int from_zext_lshft1 = (ir - (ir>>8<<8))<<1;;

	 
	if(GetDRMUX(microinstruction))
		DR = 7;
	else 
		DR = (ir>>9) - (ir>>12<<3);
		
	if(GetSR1MUX(microinstruction))
		from_sr1out = CURRENT_LATCHES.REGS[(ir>>6) - (ir>>9<<3)];
	else
		from_sr1out = CURRENT_LATCHES.REGS[(ir>>9) - (ir>>12<<3)]; 
		
	if(SR2mux)
		from_SR2mux =  signex(ir - (ir>>5<<5), 5, 16);
	else
		from_SR2mux = from_sr2out;
		
	if(GetADDR1MUX(microinstruction))
			from_addr1mux = from_sr1out;
	else 
			from_addr1mux = CURRENT_LATCHES.PC;
	 
	if(GetADDR2MUX(microinstruction) == 0)
			from_lshft1 = 0;
		if(GetADDR2MUX(microinstruction) == 1)
			from_lshft1 = signex(ir - (ir>>6<<6), 6 , 16);
		if(GetADDR2MUX(microinstruction) == 2)
			from_lshft1 = signex(ir - (ir>>9<<9), 9 , 16);
		if(GetADDR2MUX(microinstruction) == 3)
			from_lshft1 = signex(ir - (ir>>11<<11), 11, 16);
		
		
		if(GetLSHF1(microinstruction))
			from_address_adder = Low16bits((from_lshft1<<1) + from_addr1mux);
		else 
			from_address_adder = Low16bits(from_lshft1 + from_addr1mux);		
			
		
		if(GetDEC_R6(microinstruction))
			NEXT_LATCHES.REGS[6] = CURRENT_LATCHES.REGS[6] - 2;
		if(GetINC_R6(microinstruction))
			NEXT_LATCHES.REGS[6] = CURRENT_LATCHES.REGS[6] + 2;	
		
			

			
			

    if(GetGATE_PC(microinstruction))
			latch_to_bus = CURRENT_LATCHES.PC;			
		
	else if(GetGATE_MDR(microinstruction))
	{
	   if(GetDATA_SIZE(microinstruction))	
		latch_to_bus = CURRENT_LATCHES.MDR;
		else
		{
			if(mar0)
			latch_to_bus = signex(CURRENT_LATCHES.MDR >> 8, 8 , 16); // upper byte
			else
			latch_to_bus = signex(CURRENT_LATCHES.MDR - (CURRENT_LATCHES.MDR >>8<<8), 8 , 16); // lower byte 
		}

	}

		
	else if(GetGATE_MARMUX(microinstruction))
	{
		if(GetMARMUX(microinstruction))
			latch_to_bus = from_address_adder;
		else
			latch_to_bus = from_zext_lshft1;
	
		}
		

	else if(GetGATE_ALU(microinstruction))
	{
		if(GetALUK(microinstruction) == 0)
				latch_to_bus =  Low16bits(from_sr1out + from_SR2mux);
			if(GetALUK(microinstruction) == 1)
				latch_to_bus = from_sr1out & from_SR2mux;
			if(GetALUK(microinstruction) == 2)
				latch_to_bus = from_sr1out ^ from_SR2mux;
			if(GetALUK(microinstruction) == 3)
				latch_to_bus = from_sr1out;	
				
	}
				
				

				

	else if(GetGATE_SHF(microinstruction))
	{
		if(d == 0)
				latch_to_bus = Low16bits(from_sr1out << amount4);
			else
			{
				if(a)
				{
					if(from_sr1out>>15 == 0)
							latch_to_bus = from_sr1out >> amount4;
					else
					{
						int sr1 = from_sr1out;
						while(amount4 > 0)
						{
							sr1 = (sr1 >> 1) + 0x8000;
							amount4 = amount4 -1;	
						}
						latch_to_bus = sr1;
					}
				}
				else
				{
					latch_to_bus = from_sr1out >> amount4;
				}
			}
	}

	else if(GetPUSH_SSP(microinstruction))
	{
		
		latch_to_bus = NEXT_LATCHES.REGS[6];
	}

	else if(GetGATE_PSR(microinstruction))
		latch_to_bus = CURRENT_LATCHES.PSR;
		
	else if(GetGATE_INTV(microinstruction))
	{
		NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[6];
		latch_to_bus = 0x0200 + (CURRENT_LATCHES.INTV << 1);
	}	
	
	else if(GetGATE_PTBR(microinstruction))
	{
		int vpn = CURRENT_LATCHES.VA >> 9;
		vpn = vpn & 0x007F;	
		latch_to_bus = CURRENT_LATCHES.PTBR + (vpn << 1);
	}

	else if(GetGATE_VA(microinstruction))
	{
		latch_to_bus = CURRENT_LATCHES.VA  & 0x01FF;// only offset;
	}

	else if(GetGATE_MAR(microinstruction))
	{
		latch_to_bus = CURRENT_LATCHES.MAR;
	}
	else if(GetGATE_PTE(microinstruction))
	{
		latch_to_bus = CURRENT_LATCHES.PTE;
	}
	else if(GetGATE_PFN(microinstruction))
	{
		latch_to_bus = CURRENT_LATCHES.PFN << 9;
	}
	else if(GetGATE_PA(microinstruction))
	{
		latch_to_bus = CURRENT_LATCHES.PA;
		translation_done = 0;
	}
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       
   BUS = latch_to_bus;

}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       
   int *microinstruction = CURRENT_LATCHES.MICROINSTRUCTION;
   int from_addr1mux;
   int ir = CURRENT_LATCHES.IR;
   int sign_bit = BUS >> 15;
   int current_mode = CURRENT_LATCHES.PSR >> 15;
   
   int mar0 = (CURRENT_LATCHES.MAR - CURRENT_LATCHES.MAR>>1<<1); //MAR[0]	
   int valid = 1;
   int protection = 1;
	if(GetLD_REG(microinstruction))
		NEXT_LATCHES.REGS[DR] = BUS;
		
	if(GetLD_BEN(microinstruction))
	{
	int ir11 = (ir >> 11) - (ir >>11 << 1);
	int ir10 = (ir >> 10) - (ir >>10 << 1);
	int ir9 = (ir >> 9) - (ir >>9 << 1);	
	 NEXT_LATCHES.BEN = (ir11 & CURRENT_LATCHES.N) | (ir10 & CURRENT_LATCHES.Z) | (ir9 & CURRENT_LATCHES.P);
	}
	if(GetLD_CC(microinstruction))
	{
		if(BUS == 0)
		{
				NEXT_LATCHES.Z = 1;
				NEXT_LATCHES.P = 0;
				NEXT_LATCHES.N = 0;	
		}
		else
		{
		if(sign_bit)
		{
				NEXT_LATCHES.Z = 0;
				NEXT_LATCHES.P = 0;
				NEXT_LATCHES.N = 1;	
		}
		else
		{
				NEXT_LATCHES.Z = 0;
				NEXT_LATCHES.P = 1;
				NEXT_LATCHES.N = 0;	
		}
		}
		
		
	}
	if(GetLD_IR(microinstruction))
		NEXT_LATCHES.IR = BUS;
		
   
	if(GetLD_MAR(microinstruction))
		{
			NEXT_LATCHES.MAR = BUS;
			
		}

	if(GetPFNSHF(microinstruction))
			NEXT_LATCHES.PA = BUS;
	if(GetVASHF(microinstruction))
			NEXT_LATCHES.PA = CURRENT_LATCHES.PA + BUS;	
	if(GetLD_PC(microinstruction))
	{
		if(GetPCMUX(microinstruction) == 0)
			NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
		if(GetPCMUX(microinstruction) == 1)
			NEXT_LATCHES.PC = BUS;
		if(GetPCMUX(microinstruction) == 2)	
			NEXT_LATCHES.PC = from_address_adder;
		if(CURRENT_LATCHES.STATE_NUMBER == 46 && unaligned_exception) 
			unaligned_exception = 0;	
	}
	


	if(GetLD_MDR(microinstruction))
	{
		if(GetMIO_EN(microinstruction))
			NEXT_LATCHES.MDR = from_memory;
			
		else{	
			if(GetDATA_SIZE(microinstruction))
				NEXT_LATCHES.MDR = BUS;
			else 
				NEXT_LATCHES.MDR = ((BUS & 0x00FF) << 8) + (BUS & 0x00FF);
		}		
	}
		
	
		if(GetLD_PSR(microinstruction))
		{   if(CURRENT_LATCHES.STATE_NUMBER == 43)	
			NEXT_LATCHES.PSR = 0x0300;
		    else
			NEXT_LATCHES.PSR = 0x8000;	
		}
	
	 if(GetLD_SSP(microinstruction))
	 	{
			NEXT_LATCHES.TEMP_R6 = CURRENT_LATCHES.REGS[6];
			NEXT_LATCHES.REGS[6] = CURRENT_LATCHES.SSP; 
		 }
		 
		
	
	if(GetLD_R6(microinstruction))
	 	{
			NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[6] + 2;	
			NEXT_LATCHES.REGS[6] = CURRENT_LATCHES.TEMP_R6;
			NEXT_LATCHES.INTV = 0; 
		 }

	if(GetLD_VA(microinstruction))
		{
			NEXT_LATCHES.VA = BUS;
		}
	if(GetLD_PTE(microinstruction))
	{
		NEXT_LATCHES.PTE = BUS;
		valid = NEXT_LATCHES.PTE & 0x0004;
		protection = NEXT_LATCHES.PTE & 0x0008; 
		reference = NEXT_LATCHES.PTE & 0x0001;
		modified = NEXT_LATCHES.PTE & 0x0002;
	}

	
//**************************************** interrupt/exception check ***************************************************
		
	if(timer_interrupt == 1 && (CURRENT_LATCHES.STATE_NUMBER == 18 || CURRENT_LATCHES.STATE_NUMBER == 19) )
		{
			timer_interrupt = 0;
			NEXT_LATCHES.PC = NEXT_LATCHES.PC - 2;
		}
int mar0_next = NEXT_LATCHES.MAR - (NEXT_LATCHES.MAR>>1<<1);
int opcode_6bits = NEXT_LATCHES.IR >> 12;	
	if(((opcode_6bits == 10) || (opcode_6bits == 11))  )
	{
			unknown_exception = 1;
			NEXT_LATCHES.PC = NEXT_LATCHES.PC - 2;
			NEXT_LATCHES.INTV = 0x05;
			NEXT_LATCHES.STATE_NUMBER = 36;
			NEXT_LATCHES.IR = 0;
		
  		  memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
	}


if(current_mode = 1  && protect_exception == 0 && CURRENT_LATCHES.INTV == 0  &&  protection == 0 && opcode_6bits!=15)
	{
		protect_exception = 1;
		NEXT_LATCHES.PC = NEXT_LATCHES.PC - 2;
		NEXT_LATCHES.INTV = 0x04;
		NEXT_LATCHES.STATE_NUMBER = 36;

  		  memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

	}
else if(((opcode_6bits != 2) &&  (opcode_6bits != 3)) && mar0_next == 1 && unaligned_exception == 0 && protect_exception == 0)
	{
		
		unaligned_exception = 1;
		NEXT_LATCHES.PC = NEXT_LATCHES.PC - 2;
		NEXT_LATCHES.INTV = 0x03;
		NEXT_LATCHES.STATE_NUMBER = 36;
		NEXT_LATCHES.IR = 0;
	
   		 memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
	}
	else if(valid == 0 && page_fault == 0)
	{	page_fault = 1;
		NEXT_LATCHES.INTV = 0x02;
		NEXT_LATCHES.STATE_NUMBER = 36;
   		 memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
	}	

	if(GetLD_PFN(microinstruction))
	{
		pfn = CURRENT_LATCHES.PTE & 0x3E00;
		pfn = pfn >> 9;
		NEXT_LATCHES.PFN = pfn;
		
		if(reference == 0) 
		NEXT_LATCHES.PTE = NEXT_LATCHES.PTE + 1; //reference bit is set to 1 
		 
		if((store_state == 3 || store_state == 7 ) && modified == 0)
		NEXT_LATCHES.PTE = NEXT_LATCHES.PTE + 0x0002; // set modified bit 

	}	
	
}

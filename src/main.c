/**********************************************************
				MIPS Simulator in C
			Huan Nguyen and Ben Herman
		ECEN 4593 Computer Organization Spring 2017
**********************************************************/

#include <stdint.h>
//r-type masks and shifts
#define opcodeMask 		0xFC000000
#define opcodeShift 	26
#define rsMask			0x03E00000
#define rsShift			21
#define rtMask			0x001F0000
#define rtShift			16
#define	rdMask			0x0000F800
#define	rdShift			11
#define	shamtMask		0x000007C0
#define	shamtShift		6
#define	functMask		0x0000003F
//additional I type mask
#define immMask			0x0000FFFF
//additional J type mask
#define addrMask		0x03FFFFFF

//Machine code comes in as a 32-bit instruction
uint32_t machCode = 0x00000000;

typedef struct {
	unsigned int instruction;
	unsigned int pcNext; 		// value of next PC address
} IFID_PIPELINE_REG;

typedef struct {
	uint8_t shamt;				// shamt 
	uint8_t regWrite;				// control line: assert when writing to a register
	uint8_t memRead;				// control line: assert when reading from data memory
	uint8_t memWrite;				// control: assert when writing to data memory
	uint8_t memToReg;				// control: assert when loading data to reg from mem
	uint8_t ALUop;				// multi-bit control lines
	uint8_t ALU_CI;				//4-bit ALU control input
	uint8_t opCode;				// opCode
	unsigned int pcNext;		// value of next PC address
	unsigned int regRSvalue;	// VALUE of register RS (not its decimal label)
	unsigned int regRTvalue;	// value of register RT
	unsigned int regRDvalue;	// value of register RD
	int 		 signExtImm;	// sign-extended immediate value from fetch stage
	unsigned int regRS;			// register RS (not its value)
	unsigned int regRT;			// register RT (NOT its value)
	unsigned int regRD;			// register RD (not its value)
	unsigned int address;		// address for jump
	unsigned int zeroExtImm;
} IDEX_PIPELINE_REG;

typedef struct {
	uint8_t opCode;				// opcode of executed instruction
	uint8_t func;				// function code of executed instr
	uint8_t memRead;				// control: assert when reading from data memory
	uint8_t memWrite;				// control: assert when writing to data memory
	unsigned int pcNext; 		// value of next PC address
	unsigned int zero;		// zero from ALU
	unsigned int resultALU;		// result from ALU operation
	unsigned int regRTvalue;	// VALUE of register RT	
	unsigned int regWB;		// decimal label of register to write to
	uint8_t memToReg;				// control: assert when loading data from mem to reg
	unsigned int address;
}  EXMEM_PIPELINE_REG;

typedef struct {
	uint8_t opCode;				// opcode of instr to go into writeback stage
	uint8_t func;				// funct code of instr
	// Don't need shamt since this register after execution
	//	uint8_t shamt;
	unsigned int pcNext;		// value of next PC address
	uint8_t regWrite;				// control line: assert when writing back to register	
	uint8_t memToReg;				// control: assert when loading from data memory to reg
	unsigned int dataMemResult; // data loaded from memory
	unsigned int resultALU;		// result from the ALU
	unsigned int regWB;			// register to write back to
} MEMWB_PIPELINE_REG;

int memory[10000];

int regFile[32];
int lo,hi;

int pc = 0;

// Declare our shadow and actual pipeline registers: 

IFID_PIPELINE_REG shadow_IFIDreg;
IFID_PIPELINE_REG IFIDreg;

IDEX_PIPELINE_REG shadow_IDEXreg;
IDEX_PIPELINE_REG IDEXreg;

EXMEM_PIPELINE_REG shadow_EXMEMreg;
EXMEM_PIPELINE_REG EXMEMreg;

MEMWB_PIPELINE_REG shadow_MEMWB_PIPELINE_REG;
MEMWB_PIPELINE_REG MEMWBreg;

// Stage functions
void instructionFetch();
void instructionDecode();
// void executeInstruction();
void memoryAccess();
void writeBack();
void move_shadow_to_reg();

void main (void) {
	instructionFetch();
	writeBack();
	instructionDecode();
	//executeInstruction();
	memoryAccess();
	move_shadow_to_reg();
}

void instructionFetch() {
	shadow_IFIDreg.instruction = memory[pc];
	shadow_IFIDreg.pcNext = pc + 4; 
}

void writeBack() {

}

void instructionDecode() {
	if  (((IFIDreg.instruction & opcodeMask) >> opcodeShift) == 0x00) { //r type
		
		shadow_IDEXreg.regRS = (IFIDreg.instruction & rsMask) >> rsShift;
		shadow_IDEXreg.regRSvalue = regFile[shadow_IDEXreg.regRS];
			
		shadow_IDEXreg.regRT = (IFIDreg.instruction & rtMask) >> rtShift;
		shadow_IDEXreg.regRTvalue = regFile[shadow_IDEXreg.regRT];
			
		shadow_IDEXreg.regRD = (IFIDreg.instruction & rdMask) >> rdShift;
		shadow_IDEXreg.regRDvalue = regFile[shadow_IDEXreg.regRD];
			
		shadow_IDEXreg.shamt = (IFIDreg.instruction & shamtMask) >> shamtShift;
		
		shadow_IDEXreg.ALUop = 0x2;
		
		switch (memory[pc] & functMask) { 
		default: return;
		// TODO
		}
	}
	else if  (((IFIDreg.instruction & opcodeMask) >> opcodeShift) == 0x2 //j type
		 ||((IFIDreg.instruction & opcodeMask) >> opcodeShift) == 0x3) {
		shadow_IDEXreg.opCode = (IFIDreg.instruction & opcodeMask) >> opcodeShift;
	
		shadow_IDEXreg.address = (IFIDreg.instruction & addrMask);	
		
		shadow_IDEXreg.ALUop = 0x3;
	}
	else { //i type
		// I-instruction format; if branch, ALUop = 1, if lw/sw, 0.
		
		shadow_IDEXreg.ALUop = 2; // I type instruction
		
		switch (IFIDreg.instruction & opcodeMask >> opcodeShift) {
			case 0x8: //addi
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU needs to add
				break;
			case 0x9: // addiu
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU needs to add
				break;
			case 0xc: // andi
				 
				shadow_IDEXreg.ALU_CI = 0; // ALU will AND
				break;
			case 0x24: // lbu
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU will need to add
				break;
			case 0x25: // lhu
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU will add
				break;
			case 0x30: // ll
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU adds
				break;
			case 0xf: // lui
				
				shadow_IDEXreg.ALU_CI = 1; // ALU will OR (upper bits)
				break;
			case 0x23: // lw
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU will add
				break;
			case 0xd: // ori
				 
				shadow_IDEXreg.ALU_CI = 1; // ALU will OR
				break;
			case 0xa: // slti
				 
				shadow_IDEXreg.ALU_CI = 7; // set on less than
				break;
			case 0xb: // sltiu
				 
				shadow_IDEXreg.ALU_CI = 7; // set on less than
				break;
			case 0x28: // sb
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU will add
				break;
			case 0x38: // sc
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU will add
				break;
			case 0x29: // sh
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU will add
				break;
			case 0x2b: // sw
				 
				shadow_IDEXreg.ALU_CI = 2; // ALU adds
				break;
			
		}
		
		shadow_IDEXreg.regRS = (IFIDreg.instruction & rsMask) >> rsShift;
		shadow_IDEXreg.regRSvalue = regFile[shadow_IDEXreg.regRS];
			
		shadow_IDEXreg.regRT = (IFIDreg.instruction & rtMask) >> rtShift;
		shadow_IDEXreg.regRTvalue = regFile[shadow_IDEXreg.regRT];
		
		shadow_IDEXreg.signExtImm = (int) (IFIDreg.instruction & immMask);
		shadow_IDEXreg.zeroExtImm = (uint32_t) (IFIDreg.instruction & immMask);
		
		if (shadow_IDEXreg.opCode == 4 && (shadow_IDEXreg.regRSvalue == shadow_IDEXreg.regRTvalue)) {
			shadow_IDEXreg.pcNext = pc + 4 + shadow_IDEXreg.signExtImm;
		}
		if (shadow_IDEXreg.opCode == 5 && (shadow_IDEXreg.regRSvalue != shadow_IDEXreg.regRTvalue)) {
			shadow_IDEXreg.pcNext = pc + 4 + shadow_IDEXreg.signExtImm;
		}
		
		
	}

shadow_IDEXreg.opCode = IFIDreg.instruction & opcodeMask;
}
/*
void executeInstruction() {
	switch(IDEXreg.ALUop) {
		
		case : // add: R[rd] = R[rs] + R[rt]
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;
		
		case : // addi: R[rt] = = R[rs] + signExtImm 
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue+ IDEX_reg.signExtImm;
			shadow_EXMEMreg.regWB = IDEXreg.regRT;
			break;
			
		case : // addiu: R[rt] = R[rs] + signExtImm
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + (unsigned int)IDEXreg.signExtImm;
			shadow_EXMEMreg.regWB = IDEXreg.regRT;
			break;
			
		case : // and: R[rd] = R[rs] & R[rt]
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue & IDEX_reg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;
		
		case : //andi: R[rt] = R[rs] & ZeroExtImm
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue & IDEX_reg.zeroExtImm;
			shadow_EXMEMreg.regWB = IDEXreg.regRT;
			
		case : // beq: if(R[rs] = R[rt]) PC = PC + 4 + BranchAddr
			if (IDEX_reg.regRSvalue == IDEX_reg.regRTvalue) {
				shadow_EXMEMreg.resultALU = IDEX_reg.pcNext + IDEX_reg.address;
				shadow_EXMEMreg.pcNext = shadow_EXMEMreg.resultALU;
			}
			break;
		case : // bne: if (R[rs] != R[rt]) PC = PC + 4 + BranchAddr
			if (IDEX_reg.regRSvalue != IDEX_reg.regRTvalue) {
				shadow_EXMEMreg.resultALU = IDEX_reg.pcNext + IDEX_reg.address;
				shadow_EXMEMreg.pcNext = shadow_EXMEMreg.resultALU;
			}
			break;
		case : // j: PC = JumpAddr
			shadow_EXMEMreg.pcNext = IDEX_reg.address;
			break;
		case : // jal: R[31] = PC + 8; PC = JumpAddr
			regFile[31] = IDEX_reg.pcNext + 2;
			shadow_EXMEMreg.pcNext = IDEX_reg.address;  
			break;
		case : // jr: PC = R[rs]
			shadow_EXMEMreg.pcNext = IDEX_reg.regRSvalue;
			break;
		case: // lbu: R[rt] = {24'b0,M[R[rs]+SignExtImm](7:0)}
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regWB = IDEX_reg.regRT;
			break;
		case: // lhu: R[rt] = {16'b0,M[R[rs]+SignExtImm](15:0)}
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regWB = IDEX_reg.regRT;
			break;
		case: // ll: R[rt] = M[R[rs]+SignExtImm]
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regWB = IDEX_reg.regRT;
			break;
		case: // lui: R[rt] = {imm, 16'b0} 
			shadow_EXMEMreg.resultALU = IDEX_reg.signExtImm & 0x0000FFFF;
			break;
		case: // lw: R[rt] = M[R[rs]+SignExtImm]
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regWB = IDEX_reg.regRT;
			break;
		case: // nor: R[rd] = ~(R[rs]|R[rt])
			shadow_EXMEMreg.resultALU = ~(IDEX_reg.regRSvalue | IDEX_reg.regRTvalue);
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // or: R[rd] = R[rs] | R[rt]
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue | IDEX_reg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // ori: R[rt] = R[rs] | ZeroExtImm
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue | IDEX_reg.zeroExtImm; // TODO change from passing in zeroExtImm to calculating it in EX stage???
			shadow_EXMEMreg.regWB = IDEX_reg.regRT;
			break;
		case: // slt: R[rd] = (R[rs] < R[rt]) ? 1: 0
			if (IDEX_reg.regRSvalue < IDEX_reg.regRTvalue) shadow_EXMEMreg.resultALU = 1;
			else shadow_EXMEMreg.resultALU = 0;
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // slti: R[rt] = ( R[rs] < SignExtImm)? 1 : 0
			if (IDEX_reg.regRSvalue < IDEX_reg.signExtImm) shadow_EXMEMreg.resultALU = 1;
			else shadow_EXMEMreg.resultALU = 0;
			shadow_EXMEMreg.regWB = IDEX_reg.regRT;
			break;
		case: // TODO mips green sheet same as slti?? 
			break;
		case: // sltu: R[rd] = (R[rs] < R[rt])? 1: 0
			if (IDEX_reg.regRSvalue < IDEX_reg.regRTvalue) shadow_EXMEMreg.resultALU = 1;
			else shadow_EXMEMreg.resultALU = 0;
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // sll: R[rd] = R[rt] << shamt
			shadow_EXMEMreg.resultALU = IDEX_reg.regRTvalue << IDEX_reg.shamt;
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // srl: R[rd] = R[rt] >> shamt
			shadow_EXMEMreg.resultALU = IDEX_reg.regRTvalue >> IDEX_reg.shamt;
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // sb: M[R[rs]+SignExtImm](7:0) = R[rt](7:0)
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regRTvalue = IDEX_reg.regRTvalue;
			// TODO bitmask here or in mem stage?
			break;
		case: // sc TODO??? atomic??
			break;
		case: // sh: M[R[rs]+SignExtImm](15:0) = R[rt](15:0)
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regRTvalue = IDEX_reg.regRTvalue;
			// TODO do the bit mask here or in MEM stage?
			break;
		case: // sw: M[R[rs] + SignExtImm] = R[rt]
			shadow_EXMEMreg.resultALU = IDEX_reg.regRSvalue + IDEX_reg.signExtImm;
			shadow_EXMEMreg.regRTvalue = IDEX_reg.regRTvalue;
			break;
		case: // sub: R[rd] = R[rs] - R[rt]
			shadow_EXMEMreg.resultALu = IDEX_reg.regRSvalue - IDEX_reg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
		case: // subu R[rd] = R[rs] - R[rt] TODO same as sub???
			break;
		case: // div Lo=R[rs]/R[rt]; Hi=R[rs]%R[rt]
			lo = IDEX_reg.regRSvalue / IDEX_reg.regRTvalue;
			hi =  IDEX_reg.regRSvalue % IDEX_reg.regRTvalue;
			break;
		case: // divu Lo=R[rs]/R[rt]; Hi=R[rs]%R[rt]
			lo = (uint32_t)IDEX_reg.regRSvalue / (uint32_t)IDEX_reg.regRTvalue;
			hi =  (uint32_t)IDEX_reg.regRSvalue % (uint32_t)IDEX_reg.regRTvalue;
			break;
		case: //mfhi R[rd] = Hi
			shadow_EXMEMreg.regRDvalue = hi;
			break;
		case: //mflo
			shadow_EXMEMreg.regRDvalue = lo;
			break;
		case: //mult {Hi,Lo} = R[rs]*R[rt]
			lo = IDEX_reg.regRSvalue * IDEX_reg.regRTvalue;
			hi = lo;
			break;
		case: //multu {Hi,Lo} = R[rs]*R[rt]
			lo = (uint32_t)IDEX_reg.regRSvalue * (uint32_t)IDEX_reg.regRTvalue;
			hi = lo;
			break;
		case: //sra R[rd] = R[rt]>>>shamt
			if (IDEX_reg.regRTvalue[31] == 1) { //if MSB is 1 (negative), we must one-fill instead of zero-fill
				shadow_EXMEMreg.resultALU = IDEX_reg.regRTvalue >> IDEX_reg.shamt;
				shadow_EXMEMreg.resultALU += (int)(shadow_EXMEMreg.resultALU + (0x1111111100000000 >> IDEX_reg.shamt));
			}
			else {
				shadow_EXMEMreg.resultALU = IDEX_reg.regRTvalue >> IDEX_reg.shamt;
			}
			shadow_EXMEMreg.regWB = IDEX_reg.regRD;
			break;
	

	}
	// End switch(IDEX_reg.ALUop)
}
*/

void memoryAccess() {

}

void move_shadow_to_reg() {

}











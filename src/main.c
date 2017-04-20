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
	uint8_t func;				// funct field
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
	unsigned int regRD;			// register RD (not its value)
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
	unsigned int regRD;			// register RD (not its value)
	unsigned int regWB;			// register to write back to
} MEMWB_PIPELINE_REG;

extern int memory[1200];

int regFile[32];
int lo,hi;

extern int pc = 0;

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

		if (shadow_IDEXreg.opCode == 3) {
			regFile[31] = pc + 8;
		}
		pc = (pc && 0xF0000000) + (shadow_IDEXreg.address << 2);
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

void executeInstruction() {

	if ((IDEXreg.regWrite && IDEXreg.regRD != 0) && (EXMEMreg.regRD == IDEXreg.regRS)) {
		//shadow_IDEXreg.regRTvalue
		//Forward here EX hazard
		//ForwardA = 10;
	}
	if ((IDEXreg.regWrite && IDEXreg.regRD != 0) && (EXMEMreg.regRD == IDEXreg.regRT)) {
		//Forward here EX hazard
		//ForwardB = 10;
	}
	switch(IDEXreg.opCode) { // determine R-format or specific instruction

	case 0: // instruction is R-format
		switch(IDEXreg.func) { // determine which R-format function

		case 0x00: // nop
			// Do nothing
			break;

		case 0x20: // add: R[rd] = R[rs] + R[rt]
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue + IDEXreg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x21: // addu: R[rd] = R[rs] + R[rt]
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue + IDEXreg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x24: // and: R[rd] = R[rs] & R[rt]
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue & IDEXreg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x08: // jr: PC = R[rs]
			// TODO set pcNext or actual PC?
			break;

		case 0x0b: // movn: if rt != 0, move rs into rd
			if (IDEXreg.regRTvalue != 0) {
				shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue;
				shadow_EXMEMreg.regWB = IDEXreg.regRD;
			}
			// Else: do nothing
			break;

		case 0x0a: // movz: if rt = 0, move rs into rd
			if (IDEXreg.regRTvalue ==0 ) {
				shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue;
				shadow_EXMEMreg.regWB = IDEXreg.regRD;
			}
			// Else: do nothing
			break;

		case 0x27: // nor: R[rd] = ~(R[rs] | R[rt])
			shadow_EXMEMreg.resultALU = ~(IDEXreg.regRSvalue & IDEXreg.regRTvalue);
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x25: // or: R[rd] = R[rs] | R[rt]
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue | IDEXreg.regRTvalue;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x00: // sll: rd <- rt << shamt
			// Shift rt's value left by shamt
			shadow_EXMEMreg.resultALU = IDEXreg.regRTvalue << IDEXreg.shamt;
			// The write-back register is rd from previous stage
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x2a: // slt: rd <- (rs < rt)
			if (IDEXreg.regRSvalue < IDEXreg.regRTvalue)
				shadow_EXMEMreg.resultALU = 1;
			else shadow_EXMEMreg.resultALU = 0;
			shadow_EXMEMreg.regWB = IDEXreg.regRD; // Write the result to rd
			break;

		case 0x2b: // sltu: rd <- (rs_unsigned < rt_unsigned)
			if ( ((uint32_t)IDEXreg.regRSvalue) < ((uint32_t)IDEXreg.regRTvalue) )
				shadow_EXMEMreg.resultALU = 1;
			else shadow_EXMEMreg.resultALU = 0;
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x02: // srl: rd <- rt >> shamt
			// Shift rt's value right by shamt
			shadow_EXMEMreg.resultALU = IDEXreg.regRTvalue >> IDEXreg.shamt;
			// Write-back register is rd from previous stage
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x22: // sub: rd <- rs - rt
			// TODO handle overflow
			// Subtract rt from rs
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue - IDEXreg.regRTvalue;
			// Write-back register is rd
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x23: // subu: rd <- rs - rt
			// Name is a misnomer; the values aren't unsigned - the instruction is
			// intended for unsigned arithmetic, such as address arithmetic or arith-
			// metic environments that ignore overflow, such as C language arithmetic.

			// Subtract rt from rs
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue - IDEXreg.regRTvalue;
			// Write-back register is rd
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;

		case 0x26: // xor: rd <- rs XOR rt
			// XOR rs and rt, result into resultALU in pipeline reg
			shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue ^ IDEXreg.regRTvalue;
			// Write-back reg is rd
			shadow_EXMEMreg.regWB = IDEXreg.regRD;
			break;
		} // end R-format funct switch

	case 0x08: // addi: rt <- rs + sign_ext_immediate
		// add sign-extended immediate to rs, result into shadow pipeline reg
		shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue + IDEXreg.signExtImm;
		// Write-back reg is rt
		shadow_EXMEMreg.regWB = IDEXreg.regRT;
		break;

	case 0x09: // addiu: rt <- rs + sign_ext_immediate
		// TODO see Evernote
		// Do the operation
		shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue + IDEXreg.signExtImm;
		// Write-back reg is rt
		shadow_EXMEMreg.regWB = IDEXreg.regRT;
		break;

	case 0x0c: // andi: rt <- rs & zero_ext_immediate
		// Perform operation
		shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue & IDEXreg.zeroExtImm;
		// Write-back reg is rt
		shadow_EXMEMreg.regWB = IDEXreg.regRT;
		break;

	case 0x0e: // xori: rt <- rs XOR zero_ext_immediate
		// Perform operation
		shadow_EXMEMreg.resultALU = IDEXreg.regRSvalue ^ IDEXreg.zeroExtImm;
		// Write-back reg is rt
		shadow_EXMEMreg.regWB = IDEXreg.regRT;
	} // end opcode switch
}


void memoryAccess() {
	if ((MEMWBreg.regWrite && MEMWBreg.regRD != 0) && (MEMWBreg.regRD == IDEXreg.regRS)) {
		//Forward here MEM hazard
		//ForwardA = 01;
	}
	if ((MEMWBreg.regWrite && MEMWBreg.regRD != 0) && (MEMWBreg.regRD == IDEXreg.regRT)) {
		//Forward here MEM hazard
		//ForwardB = 01;
	}

}

void move_shadow_to_reg() {

}











/*
 * main.c
 *
 *  Created on: Apr 9, 2017
 *      Author: ben
 */

#include <stdio.h>
#include "loadPrg.h"

void loadPrg(int * memAddr, int * regFile, int * pc) {

	int prg1[] = {
		0x00000bb8,	//	$sp = 3000
		0x00000bb8,	//	$fp = 3000
		0x00000000,
		0x00000000,
		0x00000000,
		0x0000008c,	//	$pc = 140
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x18c00008,   // 	blez	a2,400854 <copy_array+0x24>
		0x00801025,   // 	move	v0,a0
		0x00063080,   // 	sll	a2,a2,0x2
		0x00862021,   // 	addu	a0,a0,a2
		0x8c430000,   // 	lw	v1,0(v0)
		0xaca30000,   // 	sw	v1,0(a1)
		0x24420004,   // 	addiu	v0,v0,4
		0x1444fffc,   // 	bne	v0,a0,400840 <copy_array+0x10>
		0x24a50004,   // 	addiu	a1,a1,4
		0x03e00008,   // 	jr	ra
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x1ca00018,   // 	bgtz	a1,4008c0 <bubble_sort+0x64>
		0x00005825,   // 	move	t3,zero
		0x03e00008,   // 	jr	ra
		0x00001025,   // 	move	v0,zero
		0x8c660000,   // 	lw	a2,0(v1)
		0x8c670004,   // 	lw	a3,4(v1)
		0x00e6402a,   // 	slt	t0,a3,a2
		0x11000004,   // 	beqz	t0,40088c <bubble_sort+0x30>
		0x00000000,   // 	nop
		0xac670000,   // 	sw	a3,0(v1)
		0xac660004,   // 	sw	a2,4(v1)
		0x240a0001,   // 	li	t2,1
		0x24630004,   // 	addiu	v1,v1,4
		0x1469fff6,   // 	bne	v1,t1,40086c <bubble_sort+0x10>
		0x00000000,   // 	nop
		0x004c1021,   // 	addu	v0,v0,t4
		0x10000002,   // 	b	4008a8 <bubble_sort+0x4c>
		0x00aa580a,   // 	movz	t3,a1,t2
		0x00a05825,   // 	move	t3,a1
		0x256b0001,   // 	addiu	t3,t3,1
		0x0165182a,   // 	slt	v1,t3,a1
		0x14600006,   // 	bnez	v1,4008cc <bubble_sort+0x70>
		0x01ab6023,   // 	subu	t4,t5,t3
		0x03e00008,   // 	jr	ra
		0x8c000820,		//0x00000000,   // 	nop
		0x00001025,   // 	move	v0,zero
		0x24adffff,   // 	addiu	t5,a1,-1
		0x01ab6023,   // 	subu	t4,t5,t3
		0x1980fff5,   // 	blez	t4,4008a4 <bubble_sort+0x48>
		0x00801825,   // 	move	v1,a0
		0x000c4880,   // 	sll	t1,t4,0x2
		0x00894821,   // 	addu	t1,a0,t1	0x8c190812,	//		flush memory location for testing
		0x1000ffe3,   // 	b	40086c <bubble_sort+0x10>
		0x00005025,   // 	move	t2,zero
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x28a20002,   // 	slti	v0,a1,2
		0x14400029,   // 	bnez	v0,400990 <insertion_sort+0xac>
		0x248e0004,   // 	addiu	t6,a0,4
		0x0080c025,   // 	move	t8,a0
		0x24190004,   // 	li	t9,4
		0x00001025,   // 	move	v0,zero
		0x10000016,   // 	b	400958 <insertion_sort+0x74>
		0x240f0001,   // 	li	t7,1
		0xac690000,   // 	sw	t1,0(v1)
		0x24420001,   // 	addiu	v0,v0,1
		0x104d000c,   // 	beq	v0,t5,400940 <insertion_sort+0x5c>
		0xacc80000,   // 	sw	t0,0(a2)
		0x006c1821,   // 	addu	v1,v1,t4
		0x00831821,   // 	addu	v1,a0,v1
		0x8ce8fffc,   // 	lw	t0,-4(a3)
		0x00cb3021,   // 	addu	a2,a2,t3
		0x24e7fffc,   // 	addiu	a3,a3,-4
		0x8ce9fffc,   // 	lw	t1,-4(a3)
		0x0109502a,   // 	slt	t2,t0,t1
		0x1540fff4,   // 	bnez	t2,400904 <insertion_sort+0x20>
		0x00863021,   // 	addu	a2,a0,a2
		0x10000003,   // 	b	400948 <insertion_sort+0x64>
		0x25ef0001,   // 	addiu	t7,t7,1
		0x01a01025,   // 	move	v0,t5
		0x25ef0001,   // 	addiu	t7,t7,1
		0x25ce0004,   // 	addiu	t6,t6,4
		0x27180004,   // 	addiu	t8,t8,4
		0x10af0010,   // 	beq	a1,t7,400994 <insertion_sort+0xb0>
		0x27390004,   // 	addiu	t9,t9,4
		0x19e0fffa,   // 	blez	t7,400944 <insertion_sort+0x60>
		0x01c01825,   // 	move	v1,t6
		0x8dc80000,   // 	lw	t0,0(t6)
		0x8f090000,   // 	lw	t1,0(t8)
		0x0109382a,   // 	slt	a3,t0,t1
		0x10e0fff5,   // 	beqz	a3,400944 <insertion_sort+0x60>
		0x03003025,   // 	move	a2,t8
		0x004f6821,   // 	addu	t5,v0,t7
		0x01c03825,   // 	move	a3,t6
		0x272cfffc,   // 	addiu	t4,t9,-4
		0x018e6023,   // 	subu	t4,t4,t6
		0x272bfff8,   // 	addiu	t3,t9,-8
		0x1000ffde,   // 	b	400904 <insertion_sort+0x20>
		0x01785823,   // 	subu	t3,t3,t8
		0x00001025,   // 	move	v0,zero
		0x03e00008,   // 	jr	ra
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x18c0000e,   // 	blez	a2,4009dc <check_result+0x3c>
		0x00801825,   // 	move	v1,a0
		0x00063080,   // 	sll	a2,a2,0x2
		0x00862021,   // 	addu	a0,a0,a2
		0x00001025,   // 	move	v0,zero
		0x24060001,   // 	li	a2,1
		0x8c670000,   // 	lw	a3,0(v1)
		0x8ca80000,   // 	lw	t0,0(a1)
		0x00e83826,   // 	xor	a3,a3,t0
		0x00c7100b,   // 	movn	v0,a2,a3
		0x24630004,   // 	addiu	v1,v1,4
		0x1464fffa,   // 	bne	v1,a0,4009b8 <check_result+0x18>
		0x24a50004,   // 	addiu	a1,a1,4
		0x03e00008,   // 	jr	ra
		0x00000000,   // 	nop
		0x03e00008,   // 	jr	ra
		0x00001025,   // 	move	v0,zero
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x00000000,   // 	nop
		0x27bdf7f8,   // 	addiu	sp,sp,-2056
		0xafbf0804,   // 	sw	ra,2052(sp)
		0xafb30800,   // 	sw	s3,2048(sp)
		0xafb207fc,   // 	sw	s2,2044(sp)
		0xafb107f8,   // 	sw	s1,2040(sp)
		0xafb007f4,   // 	sw	s0,2036(sp)
		0x8c420000,   // 	lw	v0,0(v0)
		0xafa207ec,   // 	sw	v0,2028(sp)
		0x240600fa,   // 	li	a2,250
		0x27b10404,   // 	addiu	s1,sp,1028
		0x02202825,   // 	move	a1,s1
		0x27b0001c,   // 	addiu	s0,sp,28
		0x02002025,   // 	move	a0,s0
		0x0c00000a,   // 	jal	10 <copy_array>
		0x00000000,   // 	nop
		0x240500fa,   // 	li	a1,250
		0x02002025,   // 	move	a0,s0
		0x0c00001e,   // 	jal	30 <bubble_sort>
		0x00000000,   // 	nop
		0x00409025,   // 	move	s2,v0
		0x240500fa,   // 	li	a1,250
		0x02202025,   // 	move	a0,s1
		0x0c000046,   // 	jal 70 <insertion_sort>
		0x00000000,   // 	nop
		0x00409825,   // 	move	s3,v0
		0x240600fa,   // 	li	a2,250
		0x02202825,   // 	move	a1,s1
		0x02002025,   // 	move	a0,s0
		0x0c000078,   // 	jal	120 <check_result>
		0x00000000,   // 	nop
		0x0240a020,	  // 	move $s32 to $s4 to be stored into result memory location
		0x0272902a,   // 	slt	s2,s3,s2
		0x16400003,   // 	bnez	s2,400a90 <main+0xac>
		0x3c03000f,   // 	lui	v1,0xf
		0x24634240,   // 	addiu	v1,v1,16960
		0x00431021,   // 	addu	v0,v0,v1
		0xac14001c,	  //	sw $s4, 28($zero)	store number of passes through bubble sort
		0xac130020,	  //	sw $s3, 32($zero) 	store number of passes through insertion sort
		0xac020024,	  //	sw $v0, 36($zero)	if 0, the two arrays are equal, 1 they are not equal
		0x8c0804b0,	  //	lw $t0, 1200($zero)	Using the data in memory location 300 to verify sort
		0xac080018,	  //	sw $t0, 24($zero)
		0x8c001018,	  //	flush cache to memory to view via memory
		0x8c00101c,	  //	flush cache to memory to view via memory
		0x8c001020,	  //	flush cache to memory to view via memory
		0x8c001024,	  //	flush cache to memory to view via memory
		0x00000000,	  //	nop
		0x00000000,	  //	nop
		0x00000000,	  //	nop
		0x00000000,	  //	nop
		0x00000008,   // 	jr	to PC = 0, indicating end of program
		0x00000000,	  //	nop
		0x00000000,	  //	nop
		0x00000000,	  //	nop
		0x00000000,	  //	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,    // 	nop
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x0a000000,
		0x00000d00,    // 	nop
		0x00b00000,    // 	nop
		0x000000e0,
		0x00030000,    // 	nop
		0x0f000000,    // 	nop
		0x0000f000,
		0xffffffff,
		0x0a0a0000,
		0x0f0f0f0f,
		0xf0f0f0f0,
		0x00010020,
		0xf0000000,
		0x0aa0bab0,
		0xa5a5a5a5,
		0x5a5a5a5a,
		0x0acccbaa,
		0x00000000,
		0xffffffff,
		0xf034a04a,
		0xaaaaaaaa,
		0xbbbbbbbb,
		0xcccccccc,
		0xdddddddd,
		0xeeeeeeee,
		0x99999999,
		0x88888888,
		0x77777777,
		0x66666666,
		0x55555555,
		0x44444444,
		0x33333333,
		0x22222222,
		0x11111111,
		0x00000001,
		0x00000010,
		0x00000100,
		0x00001000,
		0x00010000,
		0x00100000,
		0x01000000,
		0x10000000,
		0x00000002,
		0x00000020,
		0x00000200,
		0x00002000,
		0x00020000,
		0x00200000,
		0x02000000,
		0x20000000,
		0x00000003,
		0x00000030,
		0x00000300,
		0x00003000,
		0x00030000,
		0x00300000,
		0x03000000,
		0x30000000,
		0x00000004,
		0x00000040,
		0x00000400,
		0x00004000,
		0x00040000,
		0x00400000,
		0x04000000,
		0x40000000,
		0x00000005,
		0x00000050,
		0x00000500,
		0x00005000,
		0x00050000,
		0x00500000,
		0x05000000,
		0x50000000,
		0x00000006,
		0x00000060,
		0x00000600,
		0x00006000,
		0x00060000,
		0x00600000,
		0x06000000,
		0x60000000,
		0x00000007,
		0x00000070,
		0x00000700,
		0x00007000,
		0x00070000,
		0x00700000,
		0x07000000,
		0x70000000,
		0x00000008,
		0x00000080,
		0x00000800,
		0x00008000,
		0x00080000,
		0x00800000,
		0x08000000,
		0x80000000,
		0x00000009,
		0x00000090,
		0x00000900,
		0x00009000,
		0x00090000,
		0x00900000,
		0x09000000,
		0x90000000,
		0x0000000a,
		0x000000a0,
		0x00000a00,
		0x0000a000,
		0x000a0000,
		0x00a00000,
		0x0a000000,
		0xa0000000,
		0x0000000b,
		0x000000b0,
		0x00000b00,
		0x0000b000,
		0x000b0000,
		0x00b00000,
		0x0b000000,
		0xb0000000,
		0x0000000c,
		0x000000c0,
		0x00000c00,
		0x0000c000,
		0x000c0000,
		0x00c00000,
		0x0c000000,
		0xc0000000,
		0x0000000d,
		0x000000d0,
		0x00000d00,
		0x0000d000,
		0x000d0000,
		0x00d00000,
		0x0d000000,
		0xd0000000,
		0x0000000e,
		0x000000e0,
		0x00000e00,
		0x0000e000,
		0x000e0000,
		0x00e00000,
		0x0e000000,
		0xe0000000,
		0x0000000f,
		0x000000f0,
		0x00000f00,
		0x0000f000,
		0x000f0000,
		0x00f00000,
		0x0f000000,
		0xf0000000,
		0x00000011,
		0x00001100,
		0x00110000,
		0x22000000,
		0x00000022,
		0x00002200,
		0x00220000,
		0x22000000,
		0x00000033,
		0x00003300,
		0x00330000,
		0x33000000,
		0x00000044,
		0x00004400,
		0x00440000,
		0x44000000,
		0x00000055,
		0x00005500,
		0x00550000,
		0x55000000,
		0x00000066,
		0x00006600,
		0x00660000,
		0x66000000,
		0x00000077,
		0x00007700,
		0x00770000,
		0x77000000,
		0x00000088,
		0x00008800,
		0x00880000,
		0x88000000,
		0x00000099,
		0x00009900,
		0x00990000,
		0x99000000,
		0x000000aa,
		0x0000aa00,
		0x00aa0000,
		0xaa000000,
		0x000000bb,
		0x0000bb00,
		0x00bb0000,
		0xbb000000,
		0x000000cc,
		0x0000cc00,
		0x00cc0000,
		0xcc000000,
		0x000000dd,
		0x0000dd00,
		0x00dd0000,
		0xdd000000,
		0x000000ee,
		0x0000ee00,
		0x00ee0000,
		0xee000000,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000,
		0x00000101,
		0x00001010,
		0x00010100,
		0x00101000,
		0x01010000,
		0x30300000,
		0x00000303,
		0x00003030,
		0x00030300,
		0x00303000,
		0x03030000,
		0x30300000,
		0x00000202,
		0x00002020,
		0x00020200,
		0x00202000,
		0x02020000,
		0x20200000,
		0x00000505,
		0x00005050,
		0x00050500,
		0x00505000,
		0x05050000,
		0x50500000,
		0x00000606,
		0x00006060,
		0x00060600,
		0x00606000,
		0x06060000,
		0x60600000,
		0x00000f0f,
		0x0000f0f0,
		0x000f0f00,
		0x00f0f000,
		0x0f0f0000,
		0x0000e000
	};

	int n = sizeof(prg1)/4; //get number of lines
	//printf("%d\n",n);

	for (int i = 0; i < n; i++) { //load each line into main memory
		memAddr[i] = prg1[i];
	}
	printf("Program loaded\n");
	//printf("%d\n",prg1[n-1]==memory[n-1]); //check that program loaded properly

	regFile[29] = prg1[0]; //sp is told to be 0th line, load into regFile position 29
	regFile[30] = prg1[1]; //fp is told to be 1st line, load into regFile position 30
	*pc = prg1[5]; //pc is told to be 5th line, load it into special pc int

	//printf("%x\n",*pc);


	return;
}

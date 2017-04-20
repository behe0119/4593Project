/*
 * IDCache.c
 *
 *  Created on: Apr 19, 2017
 *      Author: ben
 */

int tag;
int bIndex;
int bOffset;
int i_Size, d_Size, b_Size;
void IDCacheSetUp(int * iSize, int * dSize, int * bSize, int * writeTorB) {
	i_Size = *iSize;
	d_Size = *dSize;
	b_Size = *bSize;
	int iCache[*iSize]; //will be 64, 128, 256 bytes
	int dCache[*dSize]; //will be 512, 256, 128 bytes
	int blockSize = *bSize; //will be 16, 4, 1 lines
	int WT, WB;
	if (*writeTorB == 0) { WT = 1; WB = 0; } //writeTorB = 0 (T), or 1 (B)
	else { WT = 0; WB = 1; }
	return;
}

void IDCacheInsert() {
	int data = memory[pc];
	tag = data / i_Size;
	bIndex = (data % i_Size) / b_Size;
	bOffset = (data % i_Size) % b_Size;

}


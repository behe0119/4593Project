/*
 * IDCache.c
 *
 *  Created on: Apr 19, 2017
 *      Author: ben
 */
#include <math.h>

int tag;
int bIndex;
int bOffset;
int i_Size, d_Size, b_Size;

typedef struct {
	int dirty;
	int valid;
	int tag;
	int bIndex;
	int bOffset;
	int data;

} CACHE_ENTRY;

CACHE_ENTRY * iCache;
CACHE_ENTRY * dCache;
void IDCacheInit(int * iSize, int * dSize, int * bSize, int * writeTorB) {
	i_Size = *iSize;
	d_Size = *dSize;
	b_Size = *bSize;
	iCache = (CACHE_ENTRY *) malloc(*iSize); //will be 64, 128, 256 bytes
	dCache = (CACHE_ENTRY *) malloc(*dSize); //will be 512, 256, 128 bytes
	int blockSize = *bSize; //will be 16, 4, 1 lines
	int WT, WB;
	if (*writeTorB == 0) { WT = 1; WB = 0; } //writeTorB = 0 (T), or 1 (B)
	else { WT = 0; WB = 1; }
	return;
}

void IDcacheInsert(int memLoc, int I0orD1) {
	int data = memory[memLoc];
	tag = memLoc / i_Size;
	bIndex = (memLoc % i_Size) / b_Size;
	bOffset = (memLoc % i_Size) % b_Size;

	if (I0orD1 == 0) { ///////////////////Instruction cache insert

		for (int bOff = 0; bOff < (b_Size-1); bOff++) {
			iCache[bIndex + bOff].data = memory[memLoc + bOff];
		}
		iCache[bIndex].dirty = 1;
		iCache[bIndex].valid = 1;
		return;
	}
	else {  /////////////////////////////////Data cache insert
		if (dCache[bIndex].dirty) {
			for (int bOff = 0; bOff < (b_Size-1); bOff++) {
				cacheWriteToMain(bIndex, tag, bOff); //write data back to main memory if dirty
			}
		}
		for (int bOff = 0; bOff < (b_Size-1); bOff++) { //fill cache block now that dirty data has been written back
			dCache[bIndex + bOff].data = memory[memLoc + bOff];
		}
		dCache[bIndex].dirty = 1;
		dCache[bIndex].valid = 1;
	}
	return;
}

int IDcacheGrab(int memLoc, int I0orD1) {
	tag = memLoc / i_Size;
	bIndex = (memLoc % i_Size) / b_Size;
	bOffset = (memLoc % i_Size) % b_Size;

	if (I0orD1 == 0) { //////////////////////instruction grab
		if (iCache[bIndex].valid && iCache[bIndex + bOffset].tag == tag){ //correct data in cache so return it
			hit++;
			return iCache[bIndex + bOffset];
		}
		else { //if valid == 0 or the tag doesnt match then we have to load the block, then grab again
			miss++;
			IDcacheInsert(memLoc, I0orD1);
			return IDcacheGrab(memLoc, I0orD1);
		}
	}
	else { /////////////////////////////////data grab
		if (dCache[bIndex].valid && dCache[bIndex + bOffset].tag == tag){ //correct data in cache, return it
			hit++;
			return dCache[bIndex + bOffset];
		}
		else { //if valid == 0 or tag doesnt match then we have to load the block, then grab again
			miss++;
			IDcacheInsert(memLoc, I0orD1);
			return IDcacheGrab(memLoc, I0orD1);
		}
	}
}

void cacheWriteToMain(int bIndex, int tag, int bOff) {
	//going from tag/block index/block offset to memory loc
	//offset + index shifted + tag shifted
	int indexShift = log(b_Size)/log(2);
	int tagShift = log(d_Size / b_Size)/log(2);
	memory[bOff + (bIndex << indexShift) + (tag << tagShift)] = iCache[bIndex + bOff].data;
	return;
}


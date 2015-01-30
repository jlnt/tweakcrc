/* 
 *
 * findvectorx:
 * get 32 bits X, so that CRC32N(00000(Mtimes)X000(Ntimes))=A for any given A
 * crcn(Y) = crc(Y) ^ crc(0) (crcn is linear on GF(2)[X])
 *
 * tweakcrc: patch string at offset 'offset' so that crc32(string) == target
 * 
 * (C) Julien TINNES
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include "crc32.h"
/* real poly is 1<<32+poly */
unsigned int poly=0x04C11DB7;

char	message[8];

unsigned int	messh, messl;

/* invert bit order in each bytes (crc does that before computing the real CRC!) */
unsigned int	invbytes(unsigned int ent) {
	int	i,j;
	unsigned int ret=0;

	for (i=0; i<4; i++) {
		unsigned char	tmp, tmp2=0;
		tmp=(ent>>(8*i))&0xFF;
		//printf("tmp = %X\n", tmp);
		for (j=0; j<8; j++) {
			if (tmp & (1<<j))
					tmp2|=(0x80>>j);
		}
		//printf("tmp2 %X\n", tmp2);
		ret|=tmp2<<(8*i);
	}

	return ret;
}

unsigned int invall(unsigned int ent) {
	unsigned int	ret=0;
	ent=invbytes(ent);
	/* byte order change, slow bswap */
	ret|=ent<<24;
	ret|=ent>>24;
	ret|=(ent<<8)&0x00FF0000;
	ret|=(ent>>8)&0x0000FF00;
	return ret;
}
			
/* left shift the 64 bits number formed by *mymessh:*mymessl by count bits */
void shiftboth(unsigned int *mymessh, unsigned int *mymessl, int count) {
	
	assert(count < 32);
	if (count != 0) {
		(*mymessh)=((*mymessh)<<count)+((*mymessl)>>(32-count));
		*mymessl<<=count;
	}
}

/* find 32 bitsvector such as crcn(vector)=invbits(A) */
unsigned int findvector(unsigned int A) {
	int	i, ret;
	unsigned int mesh, mesl;
	ret=0;
	mesh=1;
	mesl=poly;
	for (i=0; i<32; i++) {
		if (A&(1<<i)) {
			ret^=mesh;
			A^=mesl;
		}
		shiftboth(&mesh, &mesl, 1);
	}
	assert(A == 0);
	return ret;
}

/* find 32 bits polynomial such as crcn(polynomial*X^(32*row))=A
 * this means that crcn (polynomnial 000000000000000 (32* row zeros))=A)
 */
unsigned int findvectorx(unsigned int A, unsigned int row) {
	A=invall(A);
	while ((row--)!=-1) 
		A=findvector(A);
	return invbytes(A);
}

unsigned int	tweakcrc(void *map, int length, unsigned int target, unsigned int offset) {
	unsigned int zcrc;
	unsigned int vector;

  assert(length > offset);
  assert( (length - offset) % 4 == 0 );

	zcrc=calc_zcrc(length);
	/* CRCN(X) = CRC(0) ^ CRC(X) */
	target^=calc_crc(map,length);
	vector=findvectorx(target, (length-offset)/4-1);
	*(unsigned int*)(map+offset)^=htonl(vector);
	return vector;

}

#ifdef TEST
int	main(void) {


	printf("Vector final %X\n", findvectorx(0xDEADBEEF,1));
	/* CRCN("\x9B\xD7\xEC\xCA\x00\x00\x00\x00")== DEADBEEF */

}
#endif

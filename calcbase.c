#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "crc32.h"
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#define MSG_LEN 1056
char	buffer[MSG_LEN];
char	zero[MSG_LEN];

/* Offset in message (casted to int) */
#define COFF 1

struct sol {
	unsigned int s;
	unsigned int c;
};

unsigned int Z;

struct sol solutions[32];

struct sol nsolutions[32];

unsigned int findvectorx(unsigned int A, unsigned int row);

int	main(void) {

	int	i;
	unsigned int zcrc;
	
	printf("CRC normal base calculator\n(C) Julien TINNES\n\n");
	
	gen_table();
	zcrc=calc_crc(buffer, sizeof(buffer));
	printf("Crc(0) (zcrc) = 0x%X\n\n", zcrc);
		
	/* Calculate CRCN base */
	for (i=0; i<32; i++) {
		assert((MSG_LEN%4) == 0);
		nsolutions[i].c=(1<<i);
		((unsigned int*)(buffer))[COFF]=nsolutions[i].s=ntohl(findvectorx(1<<i, (MSG_LEN/4)-1-COFF));
		assert(nsolutions[i].c == (zcrc^calc_crc(buffer,sizeof(buffer))) );
	}
	
	Z=ntohl(findvectorx(zcrc, (MSG_LEN/4)-1-COFF));

	/* Get corresponding CRC base */
	for (i=0; i<32; i++) {
		((unsigned int*)(buffer))[COFF]=solutions[i].s=Z^nsolutions[i].s;
		solutions[i].c=nsolutions[i].c;
		/* ASSERT */
		assert (solutions[i].c == (calc_crc(buffer, sizeof(buffer))) ); 
		/*{
			printf("Error with solution %X\n", solutions[i].c);
			exit(1);
			*/
		
	}

	for (i=0; i<32; i++) {
		((unsigned int*)(buffer))[COFF]=nsolutions[i].s;
		printf("NSolution %.08X gives NCRC %.08lX\n", nsolutions[i].s,zcrc^calc_crc(buffer,sizeof(buffer)));
		((unsigned int*)(buffer))[COFF]=solutions[i].s;
		printf(" Solution %.08X gives  CRC %.08lX\n", solutions[i].s,calc_crc(buffer,sizeof(buffer)));
		//printf("0x%X, 0x%X\n ",solutions[i].s,solutions[i].c);
	}

	printf("\n You can now use this solution (be aware that it is in your host byte order!):\n{");
	for (i=0; i<32; i++) {
		printf("0x%X%s ", solutions[i].s,i!=31?",":"");
	}
	printf("}\n\n");

	printf("\n You can now use this Nsolution (It is in BIG ENDIAN byte order!):\n[");
	for (i=0; i<32; i++) {
		printf("0x%XL%s ", htonl(nsolutions[i].s),i!=31?",":"");
	}
	printf("]\n\n");
	printf("Got big Z (your host byte order): 0x%X\n\n", Z);
	printf("mlen=%d\n",MSG_LEN);
	printf("offset=%d\n\n", COFF);		

	return 0;	
	
}

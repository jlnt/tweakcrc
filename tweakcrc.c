/* CRC message tweaker
 * (C) Julien TINNES
 *
 * Give this program a file name, a given CRC and an offset in the file, it'll patch the file
 * at this offset so that the file gets the given CRC
 */
#include <stdint.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "crc32.h"
#include <arpa/inet.h>
#include <assert.h>

char *file;
unsigned int offset=-1;
unsigned int crc;

void	report(int doperror, char *fmt, ...);

unsigned int	tweakcrc(void *map, int length, unsigned int target, unsigned int offset); 

void	usage(char *argv[]) {
	printf("Usage %s [options]\n\n"
			"Options: \n"
			"-f <file>\t Compute CRC on file\n"
			"-c <crc>\t Tweak file so that his CRC is crc\n"
			"-o <offset>\t Tweak file at offset\n", argv[0]);
	exit(1);
}

int	main(int argc, char *argv[]) {

	int c;
	int fdfile;
	void *filemap;
	struct stat filestat;
	int curcrc;
	unsigned int vector;

	printf("\nTweakCRC\n(C) Julien TINNES <julien at cr0.org>\n\n");
	
	for (;;) {
		int option_index = 0;
		static	struct option long_options[] = {
			{"file", 1, 0, 'f'},
			{"crc", 1, 0, 'c'},
			{"offset", 1, 0, 'o'},
			{0,0,0,0}
		};

		c= getopt_long(argc, argv, "f:c:o:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 0:
				report(0,"this should not happen\n");
				break;
			case 'f':
				file=optarg;
				break;
			case 'o':
				offset=strtoul(optarg, NULL, 0);
				break;
			case 'c':
				crc=strtoul(optarg, NULL, 0);
				break;
			case '?':
			case ':':
			default:
				usage(argv);
		}
	}

	if (file == 0)
		usage(argv);

	fdfile=open(file, O_RDWR);
	if (fdfile == -1) 
		report(1, "Unable to open %s:\n", file);

	if (fstat(fdfile, &filestat) == -1)
		report(1, "Unable to fstat %s:\n", file);

  if (offset == -1)
    offset = filestat.st_size%4;

	if(offset >= filestat.st_size || (filestat.st_size - offset) % 4) {
		printf("Sorry, tweakcrc wants file_length - offset to be aligned to 4\n");
		printf("This is a rather trivial limitation, spend a few minutes and fix tweakcrc, or e-mail me :)\n");
		exit(1);
	}
		
	filemap=(char *) mmap(0, filestat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fdfile, 0);

	if (filemap == MAP_FAILED)
		report(1, "Unable to mmap %s:\n", file);
	
	gen_table();
	curcrc=calc_crc(filemap, filestat.st_size);
	printf("Current CRC: 0x%X\n", curcrc);
	printf("Target  CRC: 0x%X\n", crc);
	printf("Target offset: 0x%X\n", offset);
	if (offset > filestat.st_size) {
		printf("Offset is too big\n");
		exit(1);
	}
	printf("CRC(0) for length %d: 0x%X\n", (int)filestat.st_size, (unsigned int) calc_zcrc(filestat.st_size));

	vector=tweakcrc(filemap, filestat.st_size, crc, offset);
	printf("Modification vector: 0x%X\n", vector);
	printf("New CRC: 0x%X\n", (unsigned int)calc_crc(filemap,filestat.st_size));

	munmap(filemap, filestat.st_size);
	close(fdfile);
	
	return 0;	

}

void	report(int doperror, char *fmt, ...) {

	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	if (doperror) {
		perror(0);
		exit(1);
	}
	va_end(ap);

}

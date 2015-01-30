#include "crc32.h"
static unsigned long crc_table[256];

void gen_table(void)               
{
    unsigned long crc, poly;
    int	i, j;

    poly = 0xEDB88320L;
    for (i = 0; i < 256; i++)
        {
        crc = i;
        for (j = 8; j > 0; j--)
            {
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else
                crc >>= 1;
            }
        crc_table[i] = crc;
        }
}

unsigned long calc_crc(char *p, unsigned int len)   
{
    register unsigned long crc;
    unsigned int count=0;

    crc = 0xFFFFFFFF;
    while (count < len)
        crc = (crc>>8) ^ crc_table[ (crc^p[count++]) & 0xFF ];

    return( crc^0xFFFFFFFF );
}


unsigned long calc_zcrc(unsigned int len)   
{
    register unsigned long crc;
    unsigned int count=0;

    crc = 0xFFFFFFFF;
    while (count < len) {
        crc = (crc>>8) ^ crc_table[ (crc) & 0xFF ];
	count++;
    }

    return( crc^0xFFFFFFFF );
}

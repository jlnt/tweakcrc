#ifndef CRC32_H
#define CRC32_H
void gen_table(void);
unsigned long calc_crc(char *fp, unsigned int len);
unsigned long calc_zcrc(unsigned int len);
#endif

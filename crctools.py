#!/usr/bin/env python
#
# (C) 2006 Julien TINNES <julien at cr0.org>
# exploit for securitech challenge
#

import zlib
import struct
import binascii
import sys

mlen=1056

ncrcbase=[0x6E338B93L, 0x9D6067FCL, 0x7BC7BF23L, 0xF68E7F47L, 0xEC1DFF8EL, 0x993D8FC6L, 0x737D6F56L, 0xE6FADEACL, 0x8DF3CC82L, 0x5BE1E8DEL, 0xF7C4A066L, 0xEE8941CDL, 0x9D15F241L, 0x3A2BE483L, 0x3550B9DCL, 0x2BA60362L, 0x564C07C4L, 0xED9E7F53L, 0xDA3DFFA6L, 0xF57D8F96L, 0xABFD6FF6L, 0x17FDAE37L, 0x2EFA5D6FL, 0x5CF4BBDEL, 0xF9EE0666L, 0xF2DD0DCCL, 0xA5BD6A43L, 0x4A7BD586L, 0xD5F0DBD6L, 0xEBE7C676L, 0xD6CF8DEDL, 0xED996A00L ]

def	tohex(bin):
	return binascii.hexlify(struct.pack('!l', bin))


class	Crc:

	zcrc=zlib.crc32('\x00'*mlen)
	
	def	__init__(self, string):
		
		if len(string) != mlen:
			raise	Exception('wrong string len')

		self.s=string

	def	getcrc(self,beint):
		return zlib.crc32('\x00'*4+struct.pack('>I', beint)+'\x00'*(mlen-8))

	def	gcrc(self):
		return zlib.crc32(self.s)

	def	gcrcmask(self, image):
		target=image^self.zcrc
		source=0
		
		for i in range(0,len(ncrcbase)):
			if ((1L<<i) & target):
				source^=ncrcbase[i]

		return source

	def	tweakstring(self, targetcrc):
		self.s=self.s[:4]+'\x00'*4+self.s[8:]
		mask=self.gcrcmask(self.gcrc()^targetcrc^Crc.zcrc)
		self.s=self.s[:4]+struct.pack('>I', mask)+self.s[8:]


#mycrc=Crc('toto'*(mlen/4))

#print "%X" %(mycrc.gcrcmask(0x1))

#mycrc.tweakstring(0xDEADDEADL)

#print "%X" % mycrc.gcrc(),

#sys.stdout.write(mycrc.s)


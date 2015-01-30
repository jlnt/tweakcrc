#/usr/bin/env python

import struct
import binascii

from zlib import *


def	tohex(bin):
	return binascii.hexlify(struct.pack('!l', bin))

def	crc32n(string):
	return crc32('\x00'*len(string))^crc32(string)


print tohex(crc32('\xff'*4))
print tohex(crc32n('\x00'*4))
print tohex(crc32n('tototututata'))
print tohex(crc32n('\x80\x20\x83\xB8\xED'))


import socket
import time
import struct

def sender(destinationIp,destPort):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	i = 0
	while i<100:
		start = time.time()
		time.clock()
		s.sendto(struct.pack("f", start), (destinationIp, destPort))
		while 1:
			data = s.recvfrom(1024)
			if data:
				end = time.time()
				s.sendto(bytearray(struct.pack("f", end-start))  , (destinationIp, destPort))
				i+=1
				break

	s.close()

if __name__ == '__main__':
	sender("10.10.1.2", 4041)	
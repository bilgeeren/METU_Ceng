import socket
import time
import struct
import threading


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
	senderR1 = threading.Thread(target=sender, args=("10.10.1.2", 1041)) 
	senderR2 = threading.Thread(target=sender, args=("10.10.2.1", 1042)) 
	senderR3 = threading.Thread(target=sender, args=("10.10.3.2", 1043))

	senderR1.start()
	senderR2.start()
	senderR3.start()

	senderR1.join()
	senderR2.join()
	senderR3.join()

	exit(0)
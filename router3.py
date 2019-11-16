import socket
import time
import struct
import threading


def listener(hostIp,hostPort):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.bind((hostIp, hostPort))

	myStruct = struct.Struct('f')
	while 1:
		data = s.recvfrom(1024)
		if data:
			s.sendto(b'received', data[1])
			while 1:
				time = s.recvfrom(1024)
				if time:
					time = struct.unpack('f', time[0])
					print(time[0])
					break


	s.close()


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
	listenSource = threading.Thread(target=listener, args=("10.10.3.2", 1043))  #need to keep record
	listenerR2 = threading.Thread(target=listener, args=("10.10.6.2", 4042)) # need to keep record
	destinationSender = threading.Thread(target=sender, args=("10.10.7.1", 3043)) #need to keep record

	listenSource.start()
	listenerR2.start()
	destinationSender.start()

	listenSource.join()
	listenerR2.join()
	destinationSender.join()

	exit(0)
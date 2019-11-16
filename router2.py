import socket
import time
import struct
import threading

def listener(hostIp,hostPort):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.bind((hostIp, hostPort))

	myStruct = struct.Struct('f')
	i=0
	while i<100:
		data = s.recvfrom(1024)
		if data:
			s.sendto(b'received', data[1])
			while 1:
				time = s.recvfrom(1024)
				if time:
					time = struct.unpack('f', time[0])
					print(time[0])
					i+=1
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
	listenSource = threading.Thread(target=listener, args=("10.10.2.1", 1042))  #need to keep record
	listenR1 = threading.Thread(target=listener, args=("10.10.8.2", 4041))
	r3Sender = threading.Thread(target=sender, args=("10.10.6.2", 4042))
	destinationSender = threading.Thread(target=sender, args=("10.10.5.2", 3042)) #need to keep record

	listenSource.start()
	listenR1.start()
	r3Sender.start()
	destinationSender.start()

	listenSource.join()
	listenR1.join()
	r3Sender.join()
	destinationSender.join()

	exit(0)
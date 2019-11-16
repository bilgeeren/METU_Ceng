import socket
import time
import struct

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



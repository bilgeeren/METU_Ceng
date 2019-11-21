import socket
import time
import threading


def listener(hostIp,hostPort):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.bind((hostIp, hostPort))

	i=0
	while i<100:
		data = s.recvfrom(1024)
		print(data)
		print(i)
		if data:
			s.sendto(b'received', data[1])
			i+=1

	s.close()


if __name__ == '__main__':
	listenR3 = threading.Thread(target=listener, args= ("10.10.7.1", 3043))

	listenR3.start()

	listenR3.join()

	exit(0)


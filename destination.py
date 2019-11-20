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
	listenR1 = threading.Thread(target=listener, args=("10.10.4.2", 3041)) 
	listenR2 = threading.Thread(target=listener, args=("10.10.5.2", 3042)) 
	listenR3 = threading.Thread(target=listener, args= ("10.10.7.1", 3043))

	listenR1.start()
	listenR2.start()
	listenR3.start()

	listenR1.join()
	listenR2.join()
	listenR3.join()

	exit(0)


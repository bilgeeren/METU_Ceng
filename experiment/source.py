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
	r1Listener = threading.Thread(target=listener, args=("10.10.1.1", 1041)) 
	r2Listener = threading.Thread(target=listener, args=("10.10.2.2", 4042)) 
	r3Listener = threading.Thread(target=listener, args=("10.10.3.1", 1043))

	r1Listener.start()
	r2Listener.start()
	r3Listener.start()

	r1Listener.join()
	r2Listener.join()
	r3Listener.join()

	exit(0)
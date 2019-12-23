import socket
import time
import threading

def listener(hostIp,hostPort):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((hostIp, hostPort))
    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    i=0
    while i<100:
        dataFromS = s.recvfrom(1024)
        s2.sendto(dataFromS[0], ("10.10.7.1", 3043))
        while True:
        	dataFromD = s2.recvfrom(1024)
        	if dataFromD:
		        s.sendto(b'acknowledge', dataFromS[1])
		        i+=1
		        break

    s2.close()
    s.close()


if __name__ == '__main__':
	sListener = threading.Thread(target=listener, args=("10.10.3.2", 1043))

	sListener.start()

	sListener.join()

	exit(0)

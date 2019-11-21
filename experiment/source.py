import socket
import time
import threading

def sender(destinationIp,destPort):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    i = 0
    while i<100:
        s.sendto(str(time.time()).encode(), (destinationIp, destPort))
        while True:
        	data = s.recvfrom(1024)
        	if data:
        		i+=1
        		break

    s.close()


if __name__ == '__main__':
	r3Sender = threading.Thread(target=sender, args=("10.10.3.2", 1043))
	r3Sender.start()
	r3Sender.join()

	exit(0)

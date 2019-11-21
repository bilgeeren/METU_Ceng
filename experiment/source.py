import socket
import time
import threading

def sender(destinationIp,destPort):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    i = 0
    while i<100:
        start = time.time()
        time.clock()
        s.sendto(b"helo", (destinationIp, destPort))
        while i<100:
            data = s.recvfrom(1024)
            if data:
                end = time.time()
                rtt = end-start
                i+=1
                break

    s.close()

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
	r3Listener = threading.Thread(target=sender, args=("10.10.3.2", 1043))

	r3Listener.start()
	r3Listener.join()

	exit(0)

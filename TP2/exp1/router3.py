import socket
import sys
import datetime
import time
import threading


def sender(hostIp_listen, hostPort_listen,hostIp_send, hostPort_send):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((hostIp_listen, hostPort_listen))
    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while 1:
        dataFromD = s2.recvfrom(100)
        s.sendto(dataFromD[0], (hostIp_send, hostPort_send))   

    s2.close()
    s.close()

if __name__ == '__main__':
	sListener = threading.Thread(target=listener, args=("10.10.3.2", 1043))
    dListener = threading.Thread(target=listener, args=("10.10.3.2", 1043))

	sListener.start()
    dListener.start()

	sListener.join()
    dListener.join()

	exit(0)

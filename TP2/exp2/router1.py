import socket
import sys
import datetime
import time
import threading


def sourceListener(hostIp_listen, hostPort_listen):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((hostIp_listen, hostPort_listen))
    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while 1:
        dataFromD = s.recvfrom(1000)
        if dataFromD:
            s2.sendto(dataFromD[0],  ("10.10.7.1", 3043))   

    s2.close()
    s.close()

def destListener(hostIp_listen, hostPort_listen):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((hostIp_listen, hostPort_listen))
    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while 1:
        dataFromD = s.recvfrom(60)
        if dataFromD:
            s2.sendto(dataFromD[0], ( "10.10.3.1", 1044))   

    s2.close()
    s.close()
if __name__ == '__main__':
    sListener = threading.Thread(target=sourceListener, args=("10.10.3.2", 1043))
    dListener = threading.Thread(target=destListener, args=("10.10.7.2", 3044))

    sListener.start()
    dListener.start()

    sListener.join()
    dListener.join()

    exit(0)

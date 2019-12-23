import socket
import time
import math
import threading

times = []

def listener(hostIp,hostPort):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.bind((hostIp, hostPort))

	i=0
	while i<100:
		data = s.recvfrom(1024)
		s.sendto(b'acknowledge', data[1])
		times.append(time.time() - float(data[0].decode()))
		print(time.time() - float(data[0].decode()))
		i+=1

	s.close()


def sd_calc(data):
    n = len(data)

    if n <= 1:
        return 0.0

    mean, sd = avg_calc(data), 0.0

    # calculate stan. dev.
    for el in data:
        sd += (float(el) - mean)**2
    sd = math.sqrt(sd / float(n-1))

    return sd

def avg_calc(ls):
    n, mean = len(ls), 0.0

    if n <= 1:
        return ls[0]

    # calculate average
    for el in ls:
        mean = mean + float(el)
    mean = mean / float(n)

    return mean

if __name__ == '__main__':
	listenR3 = threading.Thread(target=listener, args= ("10.10.7.1", 3043))

	listenR3.start()

	listenR3.join()

	total = 0
	for item in times:
		total+=item
	print("AVG: " + str(total/len(times)))	

	e = 1.96 * sd_calc(times) / 10
	print("e: " + str(e))

	exit(0)


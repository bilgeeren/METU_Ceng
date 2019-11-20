import socket
import time
import threading


sTimes = []
dTimes = []

def listener(hostIp,hostPort):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.bind((hostIp, hostPort))

	i=0
	while i<100:
		data = s.recvfrom(1024)
		if data:
			print(i)
			s.sendto(b'received', data[1])
			i+=1
	s.close()


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
				if(destPort == 4042):
					sTimes.append(rtt)
				if(destPort == 3042):
					dTimes.append(rtt)
				i+=1
				break
	s.close()

def findAverage(l):
	total = 0
	for item in l:
		total += item
	return total/len(l)	

if __name__ == '__main__':
	listenR3 = threading.Thread(target=listener, args=("10.10.6.1", 8080))  #need to keep record
	listenR1 = threading.Thread(target=listener, args=("10.10.8.2", 4041))
	sSender = threading.Thread(target=sender, args=("10.10.2.2", 4042))
	destinationSender = threading.Thread(target=sender, args=("10.10.5.2", 3042)) #need to keep record

	listenR3.start()
	listenR1.start()
	sSender.start()
	destinationSender.start()

	listenR3.join()
	listenR1.join()
	sSender.join()
	destinationSender.join()

	with open('link_costs.txt', 'w') as f:
		f.write("router2-source\n")
		for item in sTimes:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(sTimes))
		f.write("router2-destination\n")
		for item in dTimes:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(dTimes))

	exit(0)
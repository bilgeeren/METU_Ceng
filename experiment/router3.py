import socket
import time
import threading

sTimes = []
r2Times = []
dTimes = []

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
				if(destPort == 1043):
					sTimes.append(rtt)
				if(destPort == 3043):
					dTimes.append(rtt)
				if(destPort == 8080):
					r2Times.append(rtt)
				i+=1
				break

	s.close()

def findAverage(l):
	total = 0
	for item in l:
		total += item
	return total/len(l)	


if __name__ == '__main__':
	sSender = threading.Thread(target=sender, args=("10.10.3.1", 1043))  #need to keep record
	dSender = threading.Thread(target=sender, args=("10.10.7.1", 3043)) #need to keep record
	r2Sender = threading.Thread(target=sender, args=("10.10.6.1", 8080)) #need to keep record

	sSender.start()
	dSender.start()
	r2Sender.start()

	sSender.join()
	dSender.join()
	r2Sender.join()


	with open('link_costs.txt', 'w') as f:
		f.write("router3-source\n")
		for item in sTimes:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(sTimes))

		f.write("router3-destination\n")
		for item in dTimes:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(dTimes))

		f.write("router3-router2\n")
		for item in r2Times:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(r2Times))

	exit(0)
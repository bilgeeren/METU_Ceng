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
				if(destPort == 1041):
					sTimes.append(rtt)
				if(destPort == 4041):
					r2Times.append(rtt)
				if(destPort == 3041):
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
	sSender = threading.Thread(target=sender, args=("10.10.1.1", 1041)) #need to keep record
	r2Sender = threading.Thread(target=sender, args=("10.10.8.2", 4041)) #need to keep record
	dSender = threading.Thread(target=sender, args = ("10.10.4.2", 3041)) #need to keep record

	sSender.start()
	r2Sender.start()
	dSender.start()

	sSender.join()
	r2Sender.join()
	dSender.join()

	with open('link_costs.txt', 'w') as f:
		f.write("router1-source\n")
		for item in sTimes:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(sTimes))

		f.write("router1-destination\n")
		for item in dTimes:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(dTimes))

		f.write("router1-router2\n")
		for item in r2Times:
			f.write("%s\n" % item)
		f.write("avg = %s\n" % findAverage(r2Times))

	exit(0)

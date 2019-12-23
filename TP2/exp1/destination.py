import socket
import time
import math
import threading
import datetime
import hashlib

def calculateCheckSum(data):
    hash_md5 = hashlib.md5()
    hash_md5.update(data)
    return hash_md5.hexdigest()

def listener(hostIp,hostPort):
	totalData = ''
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.bind((hostIp, hostPort))
	ackSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


	lastReceivedPacketSeqNo = 0;

	while 1:
		packet = s.recvfrom(1000)

		if packet:
			header = packet[0][:99]
			payloadData = packet[0][99:]

			checkSum = calculateCheckSum(str(payloadData))

			header = header.strip()
			parsedHeader = header.split('#')
			currentSeqNo = parsedHeader[0]
			if str(parsedHeader[1]) == str(checkSum):
				if int(currentSeqNo) == lastReceivedPacketSeqNo +1:
					print("ehehehehehehehehe+++++++++++++++++++++++++++")
					lastReceivedPacketSeqNo = lastReceivedPacketSeqNo +1
					totalData += payloadData
			
			ack = "ACK:" + str(lastReceivedPacketSeqNo)
			ackWithPadding = ack + (60-len(ack))*" "
			ackSocket.sendto(ackWithPadding,("10.10.7.2", 3044))

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

	# total = 0
	# for item in times:
	# 	total+=item
	# print("AVG: " + str(total/len(times)))	

	# e = 1.96 * sd_calc(times) / 10
	# print("e: " + str(e))

	exit(0)


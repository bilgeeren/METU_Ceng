import socket
import sys
import datetime
import time
import threading

def calculateCheckSum(data):
    hash_md5 = hashlib.md5()
    hash_md5.update(data)
    return hash_md5.hexdigest()

def sender(destinationIp,destPort):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0.5)
  
    with open("input.txt") as f:
        lastAck = 0
        counter = 0
        resendRequired = False
        while 1:

            if(not resendRequired):     
                readData = f.read(4500)
                numOfPackets = readData/900
                counter = couter + 1
                if not readData:
                    break

            for i in range(lastAck%5+1,numOfPackets+1):
                payloadData = readData[(i-1)*900:i*900]
                if not payloadData: 
                    break
                checkSumData = calculateCheckSum(payloadData)
                header = str(lastAck + i - (lastAck%5)) + "#" + checkSumData
                padding = 99 - len(header)

                packet = header + padding*" " + payloadData
                s.sendto(packet,(destinationIp, destPort))
          

            while 1:
                try:
                    ack = s.recvfrom(60)
                except socket.timeout as e:
                    if lastAck != counter*5:
                        resendRequired = True
                        break
                    else :
                        resendRequired = False

                ack = ack.strip()
                parsedAck = ack.split(':')
                if int(lastAck +1) == int(parsedAck[1]):
                    lastAck = lastAck +1
                    resendRequired = False
                else:
                    resendRequired = True
                    break


    s.close()


if __name__ == '__main__':
	r3Sender = threading.Thread(target=sender, args=("10.10.3.2", 1043))
	r3Sender.start()
	r3Sender.join()

	exit(0)

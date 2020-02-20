import socket
import sys
import datetime
import time
import threading
import hashlib

packetArray = []
counter1 = 0
counter2 = 0

def calculateCheckSum(data):
    hash_md5 = hashlib.md5()
    hash_md5.update(data)
    return hash_md5.hexdigest()

def sender(destinationIp,destPort,routerNo):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ackSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ackSocket.bind(("10.10.3.1", 1044))
    ackSocket.settimeout(0.5)
  
    if routerNo == 1:
        lastAck = 0
    else:
        lastAck = (counter2 ) * 5 
    
    
    resendRequired = False

    while counter1 < counter2:
        if routerNo == 1:
            if(not resendRequired):     
                readData = packetArray[counter1]
                numOfPackets = len(readData)/900
                counter1 = counter1 + 1
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
                print("sending " + str(lastAck + i - (lastAck%5)))
                s.sendto(packet,(destinationIp, destPort))
            

            while 1:
                try:
                    ack = ackSocket.recvfrom(60)
                except socket.timeout as e:
                    if lastAck != counter1*5:
                        resendRequired = True
                        break
                    else :
                        resendRequired = False

                ack = ack[0].strip()
                parsedAck = ack.split(':')
                print(str(parsedAck) + " successfully sent")
                print(lastAck)
                print(counter1)
                if lastAck == counter1*5:
                    resendRequired = False
                    break
                if int(lastAck +1) <= int(parsedAck[1]):
                    lastAck = int(parsedAck[1])
                    resendRequired = False
                else:
                    resendRequired = True
                    break    
        else:
            #router 2 reverse order
            if(not resendRequired):    
                lastAck = (counter2 ) * 5  
                readData = packetArray[counter2]
                numOfPackets = len(readData)/900
                counter2 = counter2 - 1
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
                print("sending " + str(lastAck + i - (lastAck%5)))
                s.sendto(packet,(destinationIp, destPort))
            

            while 1:
                try:
                    ack = ackSocket.recvfrom(60)
                except socket.timeout as e:
                    if lastAck != (counter2+2)*5:
                        resendRequired = True
                        break
                    else :
                        resendRequired = False

                ack = ack[0].strip()
                parsedAck = ack.split(':')
                print(str(parsedAck) + " successfully sent")
                print(lastAck)
                print(counter2)
                if lastAck == (counter2+2)*5:
                    resendRequired = False
                    break
                if int(lastAck +1) <= int(parsedAck[1]):
                    lastAck = int(parsedAck[1])
                    resendRequired = False
                else:
                    resendRequired = True
                    break    


    s.close()


if __name__ == '__main__':
    with open("input.txt") as f:
        readData = f.read(4500)
        packetArray.append(readData)
        if not readData:
            break
    counter2 = len(packetArray) - 1 
    
	r1Sender = threading.Thread(target=sender, args=("10.10.3.2", 1043,1))
	r2Sender = threading.Thread(target=sender, args=("10.10.3.2", 1043,2))

	r1Sender.start()
    r2Sender.start()
	r1Sender.join()
	r2Sender.join()

	exit(0)

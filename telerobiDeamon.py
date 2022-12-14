import socket, struct, shutil, threading, sys, os, datetime, time

botID = sys.argv[1]
udpPort = sys.argv[2]
tcpPort = sys.argv[3]

udpState = 'unknown'

with open('tmp/'+botID+'.txt', 'w') as f:
    f.write( str(os.getpid()) )


sockUDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockTCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = '0.0.0.0'
server_port_UDP = int(udpPort)
server_port_TCP = int(tcpPort)

sockUDP.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sockTCP.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sockUDP.bind((server_address, server_port_UDP))
sockTCP.bind((server_address, server_port_TCP))
print "Listening on " + server_address + ":" + str(server_port_UDP) + " and :"+str(server_port_TCP)

udpState = 'socket-bound'

def listenUDP():
	while True:
		udpState = 'ready'
		data, client_address = sockUDP.recvfrom(1024)
		if len(data) < 32: continue
		data = bytearray(data)
		headerOffset = data[0]
		dataOffset = data[1]
		s = (headerOffset-2)/2
		dataSizeStr = data[2:2+s]
		dataSize, = struct.unpack('<I', dataSizeStr)
		chunkSizeStr = data[2+s:headerOffset]
		chunkSize, = struct.unpack('<I', chunkSizeStr)
		header = str(data[headerOffset:dataOffset])
		image = data[dataOffset:dataOffset+chunkSize]
		#print headerOffset, dataOffset, dataSize, header
		
	        #print 'got image data, header: '+header+', size: '+str(dataSize)+', when: '+str(datetime.datetime.now())+'\r',
	        sys.stdout.flush()

	
		if header == 'N':
			f = open("tmp/cam-"+botID+".jpg.tmp", "wb")
			f.write(image)
			f.close()
		else:
			f = open("tmp/cam-"+botID+".jpg.tmp", "ab")
                	f.write(image)
			if f.tell() == dataSize:
				#print 'swap frame!', dataSize, f.tell()
				f.close()
				shutil.copy("tmp/cam-"+botID+".jpg.tmp", "tmp/cam-"+botID+".jpg")
                	else: f.close()

def listenTCP():
	lastPoll = 0

	#print 'start listening TCP'
	sockTCP.listen(1)
	while True:
		#print 'wait for tCP connection'
		connection, client_address = sockTCP.accept()
		#print ' got connection'
		try:
        		while True:
				data = connection.recv(1024)
				if data:
					#print 'got data:', data
					if data.startswith('getCommands'):
						#print 'got getCommands!'
						data = ""
						cmdsPath = 'tmp/cmds-'+botID+'.txt'
						if os.path.isfile(cmdsPath):
							with open(cmdsPath, 'r') as file:
								data = file.read()
						#print 'send', data
						connection.sendall(data)
						lastPoll = time.time()
						open('tmp/cmds-'+botID+'.txt', 'w').close()
					if data.startswith('getStatus'):
						status = 'udp:'+udpState
						status += ';lastPoll:'+str(lastPoll)
						connection.sendall(status);
				break
		finally:
			#print 'close connection'
			connection.close()

x = threading.Thread(target=listenUDP)
x.start()
y = threading.Thread(target=listenTCP)
y.start()


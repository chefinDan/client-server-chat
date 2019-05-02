from socket import *
import sys

BUFF_SIZE = 500
buff = memoryview(bytearray(BUFF_SIZE))

PORT = int(sys.argv[1])
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', PORT))

serverSocket.listen(1)
print("chatserve listening on port {}".format(PORT))

connectionSocket, addr = serverSocket.accept()

while(True):
	bytes_recvd = connectionSocket.recv_into(buff, BUFF_SIZE)
	print('Server: {}\n'.format(bytes_recvd))
	byte_message = bytes(buff[:bytes_recvd])
	message = byte_message.decode('utf-8').strip('\x00')
	print('Server: {}'.format(message) )

	if not bytes_recvd:
		connectionSocket.close()
		break

	# view = view[bytes_recvd:] # slicing views is cheap
	# toRead -= bytes_recvd

# while(1):
# 	print('Server: bytes_recvd', bytes_recvd)

	# socket.recv_into(buffer[, nbytes[, flags]])

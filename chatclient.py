from socket import *

serverName = 'localhost'
serverPort = 30000
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName, serverPort))
sentence = 'Hello from the client'
clientSocket.send(sentence.encode())
serverResponse = clientSocket.recv(1024)
print('From server: ', serverResponse.decode())
# clientSocket.close() 

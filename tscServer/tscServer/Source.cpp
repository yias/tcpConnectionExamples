

#include <iostream>
#include <stdlib.h>
#include <io.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main() {

	// Initialize winsock

	WSADATA wsData;
	WORD ver = MAKEWORD(2,2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) {
		std::cerr << "Cannot Initialize winsock! Quitting" << std::endl;
		return 1;
	}
	std::cout << "Winsock initialized successfully" << std::endl;

 	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "Cannot create a socket! Quitting" << std::endl;
		return 2;
	}

	std::cout << "Socket created successfully" << std::endl;

	// Bindan ip address to a port and socket

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(5400);
	hint.sin_addr.s_addr = INADDR_ANY; // could also use inet_pton ...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "client socket not correct! Quitting " << std::endl;
		return 3;
	}

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXHOST];	// Service (i.e. port) the client is connect on
	ZeroMemory(host, NI_MAXHOST);	// same as memset (host, 0, NI_MAXHOST)
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " connected on port " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}


	// close listening socket

	closesocket(listening);

	// While loop: accept and echo message back to client

	char buf[4096];

	while (true) {
		ZeroMemory(buf, 4096);

		// wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "Error in recv(). Quitting" << std::endl;
			break;
		}
		if (bytesReceived == 0) {
			break;
		}



		// Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
	}


	// Close the socket

	closesocket(clientSocket);

	// Shutdown (cleanup) winsock
	WSACleanup();

	return 0;


}
#include <iostream>
#include <string>
//#include <stdio.h>
//#include <stdlib.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")


int main() {

	std::string svrIPAddress = "128.178.145.67";
	int port = 5400;

	// initialize winsock

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		std::cerr << "Cannot start winsock, Err #" << wsResult << std::endl;
		return 1;
	}

	// create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Cannot create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 2;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, svrIPAddress.c_str(), &hint.sin_addr);

	// Connect to the server

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		std::cerr << "Cannot connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 3;
	}

	// Do-while loop to send and receive data

	char buf[4096];
	std::string userInput;

	do {
		// prompt the user for some text

		std::cout << "> ";
		std::getline(std::cin, userInput);

		if (userInput.size() > 0) {
			// send text
			int sendResult = send(sock, userInput.c_str(), (int)userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				// wait for responce
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0) {
					// Echo responce to console
					std::string output(buf, 0, bytesReceived);
					std::cout << "Server said " << output.c_str() << std::endl;
				}
			}
		}
		


	} while (userInput.size() > 0);

	// Close down everything
	closesocket(sock);
	WSACleanup();

	return 0;

}
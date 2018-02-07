/*---------------------------------------------------------------------------------------
--	SOURCE FILE:		udp_svr.c -   A simple echo server using UDP
--
--	PROGRAM:			usvr.exe
--
--	FUNCTIONS:			winsock2 API
--
--	DATE:				January 6, 2008
--
--	REVISIONS:			(Date and Description)
--
--	DESIGNERS:
--
--	PROGRAMMERS:		Aman Abdulla
--
--	NOTES:
--	The program will accept UDP packets from client machines.
-- The program will read data from the client socket and simply echo it back.
---------------------------------------------------------------------------------------*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_UDP_PORT 		60006	// Default port
#define MAXLEN					65000	//Buffer length
#define TRUE					1



const char* KNOCK = "Knock knock";
const char* DOCHIRA = "dochira sama deshou ka";
const char* KANF = "KanfyooSHas";
const char* O_NKGX = "o nkgx gtj o luxmkz o ykk gtj o xkskshkx o ju gtj o atjkxyzgtj";
DWORD WINAPI WorkerThread(LPVOID lpParameter);

typedef struct CONNECTION {
	sockaddr_in client;
	sockaddr_in server;
	char recvBuf[MAXLEN];
	char sendBuf[MAXLEN];
	int serverLen;
	int clientLen;
	SOCKET socket;
} *LP_CONNECTION;

int main(int argc, char **argv)
{
	int	client_len, port, n;
	SOCKET sd;
	char	buf[MAXLEN];
	struct	sockaddr_in server, client;
	WSADATA stWSAData;
	CONNECTION conn;
	WORD wVersionRequested = MAKEWORD(2, 2);
	DWORD threadId;

	port = SERVER_UDP_PORT;


	// Initialize the DLL with version Winsock 2.2
	WSAStartup(wVersionRequested, &stWSAData);

	// Create a datagram socket
	if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Can't create a socket");
		exit(1);
	}

	// Bind an address to the socket
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("Can't bind name to socket");
		exit(1);
	}

	conn.server = server;
	conn.serverLen = sizeof(server);
	conn.socket = sd;

	HANDLE t = CreateThread(NULL, 0, WorkerThread, (LPVOID)&conn, 0, &threadId);

	DWORD exitCode;
	while (true)
	{
		GetExitCodeThread(t, &exitCode);

		if (exitCode != STILL_ACTIVE)
		{
			WSACleanup();
			exit(exitCode);
		}

		Sleep(500);
	}

	closesocket(sd);
	WSACleanup();
	exit(0);
}

DWORD WINAPI WorkerThread(LPVOID val)
{
	CONNECTION * conn = (CONNECTION *)val;
	int n;

	// Find out what port was assigned and print it
	conn->clientLen = sizeof(conn->client);
	
	if ((n = recvfrom(conn->socket, conn->recvBuf, MAXLEN, 0, (struct sockaddr *)&conn->client, &conn->clientLen)) < 0)
	{
		perror("recvfrom error");
		return 1;
	}
	if (strncmp(KNOCK, conn->recvBuf, strlen(KNOCK)) != 0) {
		perror("string does not match Knock knock");
	}

	memset(conn->sendBuf, 0, MAXLEN);
	memset(conn->recvBuf, 0, MAXLEN);

	strncpy_s(conn->sendBuf, DOCHIRA, strlen(DOCHIRA));

	if (sendto(conn->socket, conn->sendBuf, strlen(DOCHIRA), 0, (struct sockaddr *)&conn->client, conn->clientLen) < 0)
	{
		perror("sendto error");
		return 1;
	}
	memset(conn->sendBuf, 0, MAXLEN);
	memset(conn->recvBuf, 0, MAXLEN);

	if ((n = recvfrom(conn->socket, conn->recvBuf, MAXLEN, 0, (struct sockaddr *)&conn->client, &conn->clientLen)) < 0)
	{
		perror("recvfrom error");
		return 1;
	}
	if (strncmp(KANF, conn->recvBuf, strlen(KANF)) != 0) {
		perror("string does not match Kanf...");
		return 1;
	}

	strncpy_s(conn->sendBuf, O_NKGX, strlen(O_NKGX));

	if (sendto(conn->socket, conn->sendBuf, strlen(O_NKGX), 0, (struct sockaddr *)&conn->client, conn->clientLen) < 0)
	{
		perror("sendto error");
		return 1;
	}
	memset(conn->sendBuf, 0, MAXLEN);
	memset(conn->recvBuf, 0, MAXLEN);
	return 0;

}
/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	udp_clnt.c - A simple UDP client program.
--
--	PROGRAM:		uclnt.exe
--
--	FUNCTIONS:		Winsock 2 API
--
--	DATE:			January 6, 2008
--
--	REVISIONS:		(Date and Description)
--
--	DESIGNERS:
--
--	PROGRAMMERS:	Aman Abdulla
--
--	NOTES:
--	The program will send a UDP packet to a user specifed server.
--  The server can be specified using a fully qualified domain name or and
--	IP address. The packet is time stamped and then sent to the server. The
--  response (echo) is also time stamped and the delay is displayed.
---------------------------------------------------------------------------------------*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <string.h>

#pragma comment (lib, "ws2_32.lib")

#define SERVER_UDP_PORT		60006	// Default port
#define MAXLEN				65000   // Maximum Buffer length
#define DEFLEN				64		// Default Length


/*----------- Function Prototypes ------------------------*/
long delay(SYSTEMTIME t1, SYSTEMTIME t2);


DWORD WINAPI WorkerThread(LPVOID lpParameter);

const char * send1 = "knock knock";
const char * recv1 = "dochira sma deshou ka";
const char * send2 = "KanfyooSHas";
const char * recv2 = "o nkgx gtj o luxmkz o ykk gtj o xkskshkx o ju gtj o atjkxyzgt";

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
	int port = SERVER_UDP_PORT;
	int	i, j, server_len, client_len;
	SOCKET sd;
	char *pname, *host, rbuf[MAXLEN], sbuf[MAXLEN];
	struct	hostent	*hp;
	struct	sockaddr_in server, client;
	SYSTEMTIME stStartTime, stEndTime;
	WSADATA stWSAData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	DWORD threadId;

	

	// Initialize the DLL with version Winsock 2.2
	WSAStartup(wVersionRequested, &stWSAData);

	// Create a datagram socket
	if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Can't create a socket\n");
		exit(1);
	}

	host = (char *)"192.168.0.16";

	// Store server's information
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if ((hp = gethostbyname(host)) == NULL)
	{
		fprintf(stderr, "Can't get server's IP address\n");
		exit(1);
	}
	//strcpy((char *)&server.sin_addr, hp->h_addr);
	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	// Bind local address to the socket
	memset((char *)&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(0);  // bind to any available port
	client.sin_addr.s_addr = htonl(INADDR_ANY);

	CONNECTION conn;
	conn.client = client;
	conn.server = server;
	conn.serverLen = server_len;
	conn.clientLen = client_len;
	conn.socket = sd;



	CreateThread(NULL, 0, WorkerThread, (LPVOID) &conn, 0, &threadId);


	
	exit(0);
}

DWORD WINAPI WorkerThread(LPVOID val)
{
	CONNECTION * conn = (CONNECTION *)val;

	int	data_size = DEFLEN, port = SERVER_UDP_PORT;

	if (bind(conn->socket, (struct sockaddr *)&conn->client, sizeof(conn->client)) == -1)
	{
		perror("Can't bind name to socket");
		exit(1);
	}
	// Find out what port was assigned and print it
	conn->clientLen = sizeof(conn->client);
	if (getsockname(conn->socket, (struct sockaddr *)&conn->client, &conn->clientLen) < 0)
	{
		perror("getsockname: \n");
		exit(1);
	}
	printf("Port aasigned is %d\n", ntohs(conn->client.sin_port));

	if (data_size > MAXLEN)
	{
		fprintf(stderr, "Data is too big\n");
		exit(1);
	}

	



	// transmit data "knock knock"
	conn->serverLen = sizeof(conn->server);

	strcpy(conn->recvBuf, send1);
	data_size = strlen(send1);
	if (sendto(conn->socket, conn->sendBuf, data_size, 0, (struct sockaddr *)&conn->server, conn->serverLen) == -1)
	{
		perror("sendto failure");
		exit(1);
	}
	memset(conn->sendBuf, 0, MAXLEN);
	memset(conn->recvBuf, 0, MAXLEN);

	// receive data
	if (recvfrom(conn->socket, conn->recvBuf, MAXLEN, 0, (struct sockaddr *)&conn->server, &conn->serverLen) < 0)
	{
		perror(" recvfrom error");
		exit(1);
	}

	//compare to "dochira deshou sama ka"

	/*for (int i = 0; i < strlen(recv1); i++)
	{
		if (conn->recvBuf[i] != recv1[i])
		{
			printf("received invalid data from server on recv1");
			return;
		}
	}*/

	if (strncmp(recv1, conn->recvBuf, strlen(recv1)) != 0)
	{
		printf("received invalid data from server on recv1");
		return;
	}

	memset(conn->sendBuf, 0, MAXLEN);
	memset(conn->recvBuf, 0, MAXLEN);

	//send Kanf....

	strcpy(conn->recvBuf, send2);
	data_size = strlen(send2);
	if (sendto(conn->socket, conn->sendBuf, data_size, 0, (struct sockaddr *)&conn->server, conn->serverLen) == -1)
	{
		perror("sendto failure");
		exit(1);
	}

	// receive data
	if (recvfrom(conn->socket, conn->recvBuf, MAXLEN, 0, (struct sockaddr *)&conn->server, &conn->serverLen) < 0)
	{
		perror(" recvfrom error");
		exit(1);
	}

	//compare to "knock knock"


	


	if (strncmp(conn->sendBuf, conn->recvBuf, data_size) != 0)
		printf("Data is corrupted\n");

	closesocket(conn->socket);
	WSACleanup();
}

// Compute the delay between tl and t2 in milliseconds
long delay(SYSTEMTIME t1, SYSTEMTIME t2)
{
	long d;

	d = (t2.wSecond - t1.wSecond) * 1000;
	d += (t2.wMilliseconds - t1.wMilliseconds);
	return(d);
}
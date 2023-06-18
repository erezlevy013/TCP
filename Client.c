#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ADDR "127.0.0.1"
#define PORT 1234
#define BACK_LOG 5


typedef struct sockaddr_in SocAdd;
typedef struct sockaddr sockaddr;

int CreateClient()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
	{
		perror("socket failed");
		return -1;
	}
	return sock;
}
int Connection(int _sock, SocAdd serverAdd)
{
	int client_sock;
	memset(&serverAdd, 0, sizeof(serverAdd));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAdd.sin_port = htons(PORT);	

	client_sock = connect(_sock, (sockaddr *) &serverAdd, sizeof(serverAdd));
	if( client_sock < 0 ) 
	{
		perror("connect failed");	
		return -1;
	}
	return _sock;
}

int Recvfrom(int _sock)
{
	char buffer[4096];
	int read_bytes;
	read_bytes = recv( _sock, buffer, sizeof(buffer), 0 );
	if( read_bytes == 0 ) 
	{
		close(_sock);
		return -1;
	}
	if(read_bytes < 0)
	{
		perror("recvfrom failed");	
		return -2;
	}
	buffer[read_bytes] = '\0';
	printf("Client: %s \n", buffer);
	return 0;
}

int SentTo(int _sock, SocAdd _clientAdd)
{
	int sent_bytes;
	char data[] = "Hi from client";
	sent_bytes = send(_sock, data , sizeof(data) , 0);
	if (sent_bytes < 0 || sent_bytes != sizeof(data)) 
	{
		perror("sendto failed");
		return -3;
	}
	printf("Message sent.\n");
	return 0;
}

int ClientStart(int _sock)
{
	int i;
	SocAdd clientAdd;
	SocAdd serverAdd;
	_sock = Connection(_sock, serverAdd);
	if(SentTo(_sock, clientAdd) < 0 )
	{
		return -1;
	}
	if( Recvfrom(_sock) < 0 )
	{
		return -1;
	}	
	return 0;
}

int main()
{
	int sock, i;
	sock = CreateClient();
	
	ClientStart( sock );
	close(sock);
	return 0;
}

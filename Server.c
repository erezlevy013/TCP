#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>

#define ADDR "127.0.0.1"
#define PORT 1234
#define BACK_LOG 2


typedef struct sockaddr_in SocAdd;


int CreateServer()
{
	SocAdd serverAdd;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int optval = 1;

	if( sock < 0 ) 
	{
		perror("socket failed");
		return -1;
	}
	memset(&serverAdd, 0, sizeof(serverAdd));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = INADDR_ANY;
	serverAdd.sin_port = htons(PORT);

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(optval)) < 0) 
	{
		perror("reuse failed");
		return -2;
	}	
	if( bind(sock, (struct sockaddr  *) &serverAdd, sizeof(serverAdd)) < 0 )
	{
		perror("bind failed");
		return -3;
	}
	if ( listen(sock, BACK_LOG) < 0) 
	{
		perror("listen failed");
		return -4;
	}
	return sock;
}

int Connection(int _sock)
{
	unsigned int clientAdd_len;
	int client_sock;
	SocAdd clientAdd;
	
	/*memset(&clientAdd, 0, sizeof(clientAdd));*/
	clientAdd_len = sizeof(clientAdd);
	client_sock = accept(_sock, (struct sockaddr  *) &clientAdd, &clientAdd_len);
	printf("%d \n", client_sock);
	if( client_sock < 0 ) 
	{
		perror("accept failed");	
		return -1;
	}
	
	return client_sock;
}

int Recvfrom(int _sock, SocAdd _clientAdd)
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
	sent_bytes = send(_sock, "Wellcom from server", 19 , 0);

	if (sent_bytes < 0) 
	{
		perror("sendto failed");
		return -3;
	}
	printf("Message sent.\n");
	return 0;
}

int ServerStart(int _sock)
{
	SocAdd clientAdd;
	if( Recvfrom( _sock, clientAdd) < 0 )
	{
		return -1;
	}
	if(SentTo( _sock, clientAdd) < 0 )
	{
		return -1;
	}
	return 0;
}

int main()
{
	int sock, i, con;
	sock = CreateServer();
	con = Connection(sock);
	ServerStart( con );
	close(sock);
	return 0;
}

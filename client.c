#include<netinet/in.h>         // sockaddr_in 
#include<sys/types.h>          // socket 
#include<sys/socket.h>         // socket 
#include<stdio.h>              // printf 
#include<stdlib.h>             // exit 
#include<string.h>             // bzero 
    
   
#define SERVER_PORT 8000 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 


struct sockaddr_in client_addr;//客户端结构体类型（内含有端口号，IPv4地址，以及地址族的结构体）
struct sockaddr_in server_addr;
int client_socket_fd;//客户端socket返回值，用于判断
char file_serve_name[FILE_NAME_MAX_SIZE+1]; 
char file_client_local[FILE_NAME_MAX_SIZE+1];
char buffer[BUFFER_SIZE]; 
FILE *fp;



int Start_Client_Socket()// 声明并初始化一个客户端的socket地址结构 
{
  	bzero(&client_addr, sizeof(client_addr)); 
  	client_addr.sin_family = AF_INET; 
  	client_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  	client_addr.sin_port = htons(0); 
  	return 1;
}

int Creat_Socket()// 创建socket，若成功，返回socket描述符 
{
  	client_socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
  	if(client_socket_fd < 0) 
  	{ 
    		perror("创建socket失败:"); 
    		exit(1); 
  	} 
  	return 1;
}
  
int Client_Server_Bind() // 绑定客户端的socket和客户端的socket地址结构
{
  	if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)))) 
  	{ 
    		perror("Client Bind Failed:"); 
    		exit(1); 
  	} 
  	return 1;
}

int Start_Serve_Socket()// 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接 
{
  	char IP_serve_local[FILE_NAME_MAX_SIZE+1];
  	bzero(IP_serve_local, FILE_NAME_MAX_SIZE+1); 
  	printf("请输入服务器的IP地址:\t"); 
  	scanf("%s",IP_serve_local); 
  	bzero(&server_addr, sizeof(server_addr)); 
  	server_addr.sin_family = AF_INET; 
  	if(inet_pton(AF_INET, IP_serve_local, &server_addr.sin_addr) == 0) 
  	{ 
    		perror("服务器IP错误:"); 
    		exit(1); 
  	} 
  	server_addr.sin_port = htons(SERVER_PORT); 
  	return 1;
}
 
int Client_Serve_Link()// 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接 
{
  	socklen_t server_addr_length = sizeof(server_addr); 
  	if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0) 
  	{ 
    		perror("无法连接到服务器:"); 
    		exit(0); 
  	} 
   	return 1;
}

int File_LocalName_Trans()
{
  	bzero(file_serve_name, FILE_NAME_MAX_SIZE+1);
  	bzero(file_client_local, FILE_NAME_MAX_SIZE+1); 
  	printf("请输入要获取的文件在服务器的位置及名字:\t"); 
  	scanf("%s",file_serve_name); 
  	printf("请输入要保存的文件在客户端的位置及名字:\t"); 
  	scanf("%s",file_client_local); 
  	bzero(buffer, BUFFER_SIZE); 
  	strncpy(buffer, file_serve_name, strlen(file_serve_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_serve_name)); 
  	return 1;
}

int Send_File_Server_Name()// 向服务器发送要获取的位置及名称
{
  	if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0) 
  	{ 
    		perror("发送文件名称错误:"); 
    		exit(1); 
  	} 
	return 1;
}

int Write_Server_To_Client()
{
   // 打开文件，准备写入 
  	fp = fopen(file_client_local, "w"); 
  	if(NULL == fp) 
  	{ 
    		printf("File:\t%s 无法写入\n", file_serve_name); 
    		exit(1); 
  	} 
   
  // 从服务器接收数据到buffer中 
  // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
  	bzero(buffer, BUFFER_SIZE); 
  	int length = 0; 
  	while((length = recv(client_socket_fd, buffer, BUFFER_SIZE, 0)) > 0) 
  	{ 
    		if(fwrite(buffer, sizeof(char), length, fp) < length) 
    		{ 
      			printf("File:\t%s 写入失败\n", file_serve_name); 
      			break; 
    		} 
    		bzero(buffer, BUFFER_SIZE); 
  	} 
  	printf("文件:\t%s 接收成功\n", file_serve_name);
	return 1; 
}
 
int Close_File_Socket()// 接收成功后，关闭文件，关闭socket
{
  
  	close(fp); 
  	close(client_socket_fd); 
	return 1;
}


int main() 
{ 

  	Start_Client_Socket(); 
  	Creat_Socket();
 	Client_Server_Bind();
  	Start_Serve_Socket();
  	Client_Serve_Link();
  	File_LocalName_Trans();
  	Send_File_Server_Name();
  	Write_Server_To_Client();
  	Close_File_Socket();
 
  	return 0; 
}


#include<netinet/in.h>         // sockaddr_in 
#include<sys/types.h>          // socket 
#include<sys/socket.h>         // socket 
#include<stdio.h>              // printf 
#include<stdlib.h>             // exit 
#include<string.h>             // bzero 
   
#define SERVER_PORT 8000 
#define LENGTH_OF_LISTEN_QUEUE 20 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 

struct sockaddr_in server_addr; 
struct sockaddr_in client_addr;

socklen_t client_addr_length = sizeof(client_addr);       // 定义客户端的socket地址结构 

int server_socket_fd;
int new_server_socket_fd;
int opt = 1; 
int length = 0; 

char buffer[BUFFER_SIZE];
char file_name[FILE_NAME_MAX_SIZE+1];  

FILE *fp;

int Start_Server_Socket()  // 声明并初始化一个服务器端的socket地址结构 
{
  	bzero(&server_addr, sizeof(server_addr)); 
  	server_addr.sin_family = AF_INET; 
  	server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  	server_addr.sin_port = htons(SERVER_PORT); 
  	return 1;
}

int Creat_Socket()  // 创建socket，若成功，返回socket描述符 
{
  	server_socket_fd = socket(PF_INET, SOCK_STREAM, 0); 
  	if(server_socket_fd < 0) 
  	{ 
    		perror("创建socket失败:"); 
    		exit(1); 
  	} 
  	setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
	return 1;
}

int Client_Server_Bind() // 绑定客户端的socket和客户端的socket地址结构
{
  	if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
  	{ 
    		perror("Server Bind Failed:"); 
    		exit(1); 
  	} 
	return 1;
}

int Serve_Listen()
{
  	if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))) 
  	{ 
    		perror("监听失败:"); 
    		exit(1); 
  	}
	return 1; 
   
}
int Serve_Accept_Link()// 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信
{
    	new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length);     // accept函数会把连接到的客户端信息写到client_addr中 
    	if(new_server_socket_fd < 0) 
    	{ 
      		perror("Server Accept Failed:"); 
    		exit(0);
    	} 
	return 1;
}

int Send_File_Name()  //接收需要发送的文件位置及文件名
{
 	// recv函数接收数据到缓冲区buffer中 
    	bzero(buffer, BUFFER_SIZE); 
    	if(recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0) 
    	{ 
      		perror("接受文件名失败"); 
      		exit(0); 
    	} 
   	// 然后从buffer(缓冲区)拷贝到file_name中 
    	bzero(file_name, FILE_NAME_MAX_SIZE+1); 
    	strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer)); 
    	printf("要发送的文件名为：%s\n", file_name); 
	return 1;
}

int Document_Send()
{
   	// 打开文件并读取文件数据 
    	fp = fopen(file_name, "r"); 
    	if(NULL == fp) 
    	{ 
      		printf("File:%s Not Found\n", file_name); 
    	} 
    	else 
    	{ 
      		bzero(buffer, BUFFER_SIZE); 
		// 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
      		while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
      		{ 
        		if(send(new_server_socket_fd, buffer, length, 0) < 0) 
        		{ 
          			printf("Send File:%s Failed./n", file_name); 
          			exit(0); 
        		} 
        		bzero(buffer, BUFFER_SIZE); 
      		} 
      		// 关闭文件 
      		fclose(fp); 
      		printf("文件:%s 发送成功\n", file_name); 
    	} 
	return 1;
   
}

int Link_Trans()
{
  	while(1) 
	{ 
		Serve_Accept_Link();
		Send_File_Name();
		Document_Send();	
		close(new_server_socket_fd); // 关闭与客户端的连接 
	} 
	return 1;
}


int main(void) 
{ 
	Start_Server_Socket();
	Creat_Socket();
	Client_Server_Bind();  
	Serve_Listen();   
	Link_Trans();
   	close(server_socket_fd);   // 关闭监听用的socket
  	return 0; 
}


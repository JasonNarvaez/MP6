#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>

#include <errno.h>

#include "NetworkRequestChannel.h"

using namespace std;


int clientInt(const char * server, const char * port)
{
	int status;
	struct addrinfo hints,*res;
	memset(&hints, 0, sizeof hints);//clear hints before hand
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	/*cout << endl;
	cout << "LOLOLOLOLOLOL" << endl;
	cout << endl;*/
	//get address
	if ((status = getaddrinfo(server, port, &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
	}
	
	//make socket
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0)
	{
		perror ("Error creating socket\n");	
		return -1;
	}

	// connect
	if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
	{
		perror ("connect error\n");
		return -1;
	}
	
	char buf[1024];
    sprintf (buf, "hello");
    send (sockfd, buf, strlen (buf)+1, 0);
    recv (sockfd, buf, 1024, 0);
	printf ("Received %s from the server\n", buf);
}
int NetworkChannelserverInt(const char * serve, int backLog)
{
	
}

NetworkRequestChannel:: NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){
    //client channel
    int comeGuardian;//open your heart(server address)
    int sockfd;
    struct addrinfo hints; 
    struct addrinfo *res;
     
    memset(&hints, 0, sizeof hints);//clear hints before hand
    hints.ai_family = AF_UNSPEC;//don't care whether IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;//TCP stream sockets
    
    stringstream portnum;
    portnum << _port_no;
    string ports = portnum.str();
    char* port = new char[ports.length() + 1];
    strcpy(port, ports.c_str());
    const char* server_host = _server_host_name.c_str();
    if (comeGuardian = getaddrinfo(server_host, port, &hints, &res) == -1)
    {
        cerr << "Error retrieving IP address." << endl;
    }
    
    //make the socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    
	if (sockfd < 0)
	{
        //socket file descriptor error
        perror ("Error creating socket.\n");
    }
    
    //connect 
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror ("Connection error.\n");
    }
    fd = sockfd;//not sure
    cout << "You are connected to " << _server_host_name << endl;
    cout << "Sending message to " << _server_host_name << endl;
    
    //following code can be commented out, i think. cause we just connect to the server
    /*
    char buf[1024];
    sprintf (buf, "hello");
    send (sockfd, buf, strlen (buf)+1, 0);
    recv (sockfd, buf, 1024, 0);
	printf ("Received %s from the server\n", buf);
	*/
 } 
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (void *), int backlog)
{
    int sockfd;//listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *serv;
    struct sockaddr_storage their_addr;//connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;//use my IP
    
    stringstream serverNum;
    serverNum << _port_no;
    string ports = serverNum.str();
    char* port = new char[ports.length() + 1];
    strcpy(port, ports.c_str());
    
	cout << ports << endl;
	
    if ((rv = getaddrinfo(NULL, port, &hints, &serv)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        //return -1;
    }
	if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
        perror("server: socket");
		//return -1;
    }
    if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		//return -1;
	}
	
	freeaddrinfo(serv);
	if(listen(sockfd, backlog) < 0){   //maximum of 20 connections in queue
	    perror("listen");
	    exit(1);
	}
	
	printf("server: waiting for connections...\n");
	char buf [1024];
	int d = 1;
	while(1){
	    sin_size = sizeof(their_addr);
	    int *new_fd = new int;
	    pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
	    
		*new_fd = accept(sockfd, (struct sockaddr *) &their_addr,(socklen_t*) &sin_size);
		NetworkRequestChannel* n = new NetworkRequestChannel(new_fd);
	    if(new_fd < 0){
			delete new_fd;
	        perror("accept");
	        continue;
	    }
	    //inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection\n");
		//recv (*new_fd, buf, sizeof (buf), 0);
		//printf("server: received msg: %s\n", buf);
		/*
		// send 
		string s = "Hello to you";
		char *msg = new char[s.length() + 1];
		strcpy(msg, s.c_str());
		
        if (send(*new_fd, msg, strlen(msg)+1, 0) < 0)
		{
            perror("send");
        }
        */
        pthread_create(&thread, &attr,connection_handler,(void*) n);
		
		
		//if (d == 10) break;
	}
	close(sockfd);
}

NetworkRequestChannel::~NetworkRequestChannel(){
    cout<<"close Network Request Channel"<<" fd: "<<fd<<endl;
    close(fd);//close the socket
    
}

string NetworkRequestChannel::cread()
{
    char buf[msgLength];
    if (recv(fd, buf, msgLength,0) < 0)
    {
        perror("read");
    }
    string strRead = buf;
    return strRead;
}

int NetworkRequestChannel::cwrite(string _msg)
{
	if (_msg.length() >= msgLength)
	{
		perror("write");
		return -1;
	}
    const char * strung = _msg.c_str();
    if(send(fd, strung, strlen(strung)+1,0) < 0)
    {
        perror("write");
    }
}

int NetworkRequestChannel::read_fd()
{
	return fd;
}

string NetworkRequestChannel::send_request(string _request) {
  cwrite(_request);
  string s = cread();
  return s;
}

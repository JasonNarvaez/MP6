/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "NetworkRequestChannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    int msgLength;

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void handle_process_loop(NetworkRequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

string sread(int *fd)
{
    char buf[msgLength];
    recv(*fd, buf, msgLength, 0);
    string sRead = buf;
    return sRead;
}

int swrite(int *fd, string s)
{
	if (s.length() >=msgLength)
		perror("Message Too BigLOL\n");
	
	if(send(*fd, s.c_str(), s.length()+1, 0) < 0)
		perror("write"); //write to fd
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

void * handle_data_requests(void * args) {

  NetworkRequestChannel * data_channel =  (NetworkRequestChannel*)args;

  // -- Handle client requests on this channel. 
  
  //  handle_process_loop(*data_channel);

  // -- Client has quit. We remove channel.
 
  delete data_channel;
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(NetworkRequestChannel *nrc, const string & _request) {
  //cwrite(fd, "hello to you too");
  nrc->cwrite("hello to you too");
}

void process_data(NetworkRequestChannel *nrc, const string &  _request) {
  usleep(1000 + (rand() % 5000));

  nrc->cwrite(int2string(rand() % 100));
}
/*
void process_newthread(NetworkRequestChannel & _channel, const string & _request) {
  int slavesockfd;
  int error;
  nthreads ++;

  // -- Name new data channel

  string new_channel_name = "data" + int2string(nthreads) + "_";
  //  cout << "new channel name = " << new_channel_name << endl;

  // -- Pass new channel name back to client

  _channel.cwrite(new_channel_name);

  // -- Construct new data channel (pointer to be passed to thread function)
  
  NetworkRequestChannel * data_channel = new NetworkRequestChannel(new_channel_name, slavesockfd);
  //    const unsigned short _port_no, void * (*connection_handler) (int *)
  // -- Create new thread to handle request channel

  pthread_t thread_id;
  //  cout << "starting new thread " << nthreads << endl;
  if (error = pthread_create(& thread_id, NULL, handle_data_requests, data_channel)) {
    fprintf(stderr, "p_create failed: %s\n", strerror(error));
  }  

}*/

void process_request(NetworkRequestChannel *nrc, const string &Request)
{
	if (Request.compare(0,5,"hello") == 0)
		process_hello(nrc, Request); 
	else if (Request.compare(0,4,"data") == 0)
		process_data(nrc, Request);
	/*else if (Request.compare(0, 9, "newthread") == 0) {
		process_newthread(_channel, _request);
	}
	else {
		swrite("unknown request");
	}*/
}

void *connection_handler (void * arg)
{
  NetworkRequestChannel* n = (NetworkRequestChannel*) arg;
  int sockfd = n->getfd();
	//if(*fd == NULL)
      cout<<"Connection Handler ACTIVATED" << endl;
  int* fd = &sockfd;
	while(1)
	{
	  	string request = n->cread();
		//cout << " done (" << _channel.name() << ")." << endl;
		cout << "New request is " << request << endl;
		if (request.compare("quit") == 0)
	  	{
			n->cwrite("bye");
			usleep(10000);
			break;
	  	}
		process_request(n,request);
	}
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/
/*
void process_request(NetworkRequestChannel & _channel, const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(_channel, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(_channel, _request);
  }
  else if (_request.compare(0, 9, "newthread") == 0) {
    process_newthread(_channel, _request);
  }
  else {
    _channel.cwrite("unknown request");
  }

}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

 
  int backlog;
  unsigned short port;
  port = 80;
  backlog = 20;
  //  cout << "Establishing control channel... " << flush;
  //NetworkRequestChannel control_channel(port, connection_handler);
  //  cout << "done.\n" << flush;

  //handle_process_loop(control_channel);

  int opt;
    while((opt = getopt(argc, argv, "p:b:")) != -1){
  		switch (opt){
  			case 'p':{
  			  if (atoi(optarg) != 0)
  				port = atoi(optarg);
  				break;
  			}
  			case 'b':{
  			  if (atoi(optarg) != 0)
  				backlog = atoi(optarg);
  				break;
  			}
  			default:{
  				exit(EXIT_FAILURE);
  			}
  		}	
  	}
  	
  NetworkRequestChannel submit(port, connection_handler, backlog);
	submit.~NetworkRequestChannel();
}


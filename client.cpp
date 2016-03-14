/* 
    File: simpleclient.C
    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31
    Simple client main program for MP3 in CSCE 313
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
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iomanip> 
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <string>

#include <sys/select.h>

#include "NetworkRequestChannel.h"
#include "BoundedBuffer.h"
#include "semaphore.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/
//a long sentence using only c++ keywords:
//if new true friend not protected for explicit private union, break case and try using this

struct RequestThread{
	RequestThread(int n): reqno(n) { }
	int reqno;
};

int n;//amount of requests per person
int b;//size of bounded buffer
int w;//number of request channels to be handled by event handler thread

string host;
unsigned short port;

BoundedBuffer RequestBuffer;
BoundedBuffer *buffer;
BoundedBuffer joeBuffer;	//Response buffers
BoundedBuffer janeBuffer;
BoundedBuffer johnBuffer;

vector<int>joeGraph(100);	//Histograms
vector<int>janeGraph(100);	//Response int's 0-99
vector<int>johnGraph(100);

void display(vector<int>v, string name)
{
	cout << endl;
	cout << name << "'s histogram: " << endl;
	for (int i = 0; i < 10; i++)
	{
		int min = i*10;
		int max = min + 9;
		int total = 0;
		for (int j = min; j < max; j++)
			total += v[j];
		
		cout << "[" << min+1 << " - " << max+1 << "]: " << setw(15) << left << total << endl;
	}
	cout << endl;
	cout <<  "-----------------------------------------------------" << endl;
}

void * statThread(void* arg)
{
	int id = *(int*) arg;
	if(id == 0)
			for(int i = 0; i<n;i++)
			{
				string resp = joeBuffer.remove();
				//cout << "JOE RESP: " << resp << endl;
				joeGraph[atoi(resp.c_str())]+=1;
			}
	if(id == 1)
			for(int i = 0; i<n;i++)
			{
				string resp = janeBuffer.remove();
				//cout << "JANE RESP: " << resp << endl;
				janeGraph[atoi(resp.c_str())]+=1;
			}
	if(id == 2)
			for(int i = 0; i<n;i++)
			{
				string resp = johnBuffer.remove();
				//cout << "JOHN RESP: " << resp << endl;
				johnGraph[atoi(resp.c_str())]+=1;	
			}
}

void* RTF(void* arg){
	int ind = *(int*) arg;
	if(ind == 0)
			for(int i = 0; i<n;i++)
			{	//cout<<i<<endl;
				//cout<<endl<<"pushing joe smith"<<endl;
				RequestBuffer.add("data Joe Smith");
			}
	if(ind == 1)
			for(int i = 0; i<n;i++)
			{//cout<<i<<endl;
				//cout<<endl<<"pushing jane smith "<<i<<endl;
				RequestBuffer.add("data Jane Smith");
			}
	if(ind == 2)
			for(int i = 0; i<n;i++)
			{//cout<<i<<endl;
				//cout<<endl<<"pushing john doe "<<i<<endl;
				RequestBuffer.add("data John Doe");
			}
	
}

int identify(string response){
	string joe_smith = "Joe Smith";
	string jane_smith = "Jane Smith";
	string john_doe = "John Doe";
	int found;// = response.find(jane_smith);
	
	if(response.find(joe_smith) != -1)
		found = 0;
	else if(response.find(jane_smith) != -1)
		found = 1;
	else if(response.find(john_doe) != -1)
		found = 2;
	else//not found
		found = -1;
	return found;
	//cout<<found<<endl;
	//if()
	
}
/*
void* WTF(void* arg){
	//int ind = *(int*) arg;
	int ind;//identifies the person. 0 = joe smith, 1 = jane smith, 2 = john doe
	
	
	NetworkRequestChannel* rChan = (NetworkRequestChannel*) arg;//use the channel from main
	//string ch = rChan.send_request("newthread");
	//cout<<endl<<ch<<endl;
	//RequestChannel* rc = new RequestChannel(ch,RequestChannel::CLIENT_SIDE);
	// cout<<"newthread name: "<<ch<<endl;
	while(true)
	{
		
		string request = RequestBuffer.remove();
		if(request == "Stop"){break;}
		string response = rChan->send_request(request);
		ind = identify(request);
		if(ind == 0)
			joeBuffer.add(response);
		else if(ind == 1)
			janeBuffer.add(response);
		else if(ind == 2)
			johnBuffer.add(response);
	}
	rChan->send_request("quit");
	//RequestChannel* rc = new ReqChan;
	
	while(true){
		msg = retrieve from Req_BB
		resp = rc->send(msg)
		//puts resp in the right resp_buffer
	}
	
}*/

void timer_handler (int signum)
{
	static int count = 0;
 	printf ("\n*********************\ntimer expired %d times\n*********************\n", ++count);
 	display(joeGraph,"joe smith");
	display(janeGraph,"jane smith");
	display(johnGraph,"john doe");
}

void* EventHandler(void* arg){//most is pseudo-code
	
	NetworkRequestChannel* oniiChan = (NetworkRequestChannel*) arg;//use the channel from main
	//create 'w' amount of requests here?
	vector<NetworkRequestChannel*> RC(w);
	vector<string> request_name(w);//holds the name of the requests
	int removed_count = 0;
	int response_count = 0;
	//struct timeval wait = {0,10};
	
	for(int i=0;i<w;i++){//create w amount of request channels
		//string ch = oniiChan->send_request("newthread");
		RC[i] = new NetworkRequestChannel(host, port);
		//pthread_create(&wtid[i],NULL,WTF,newchan);
	}
	
	
		struct sigaction sa;
		struct itimerval timer;
		
		// Install timer_handler as the signal handler for SIGVTALRM. 
		memset (&sa, 0, sizeof (sa));
		sa.sa_handler = &timer_handler;
		sigaction (SIGALRM, &sa, NULL);
		
		// Configure the timer to expire after 250 msec... 
		timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = 250000;
		// ... and every 250 msec after that. 
		timer.it_interval.tv_sec = 0;
		timer.it_interval.tv_usec = 250000;
		// Start a REAL timer. It counts down according to wall-clock time. 
 		setitimer (ITIMER_REAL, &timer, NULL);
	
	
	while(true){
		fd_set s;
		int read = 0;
		int selected = 0;
		int max = 0;
		
		FD_ZERO(&s);//clears set s of any extraneous data
		//cout<<"Using FD_SET ==================================================================="<<endl;
		
		for(int i=0; i < w; i++)
		{								//put rfds into the set s, and find max file descriptor
			if(RC[i]->read_fd()>max) 
			{
				max=RC[i]->read_fd();
			}
										// first argument = file descriptor trying to be found in the set
			FD_SET(RC[i]->read_fd(),&s);
			//cout<<"RC["<<i<<"] read_fd(): "<<RC[i]->read_fd()<<endl;
		}
	
		//fill up channels with initial requests
		//handel the rest //break it ?
		//close it up
		
		if(removed_count < w){//send out the first requests into the channels
			for(int i=0;i<w;i++)  {
				request_name[i] = RequestBuffer.remove();
				RC[i]->cwrite(request_name[i]);//not sure about this one
				removed_count++;
			}
		}
		//cout<<"removed_count"<<removed_count<<endl;
		
		
		//cout<<"calling Select()\t";
		selected = select(max + 1, &s, NULL, NULL, NULL);
		//cout<<"\nselected: "<<selected<<endl;
		if (selected)
		{
			for(int i=0;i<w;i++){
				if(FD_ISSET(RC[i]->read_fd(), &s) ){//is the value from read_fd() in the set s?
					
					//cout<<"Iteration: "<<i<<endl;
					//if(removed_count > w){
					string request = request_name[i];//get the name of who sent the request
					//cout<<"request channel: "<<i<<endl;
					//cout<<"request: "<<request_name[i]<<endl;
					if(request == "Stop"){
						//cout<<"request was STOP"<<endl;
						removed_count++;
						//break;
						
					}
					
					else{
						string response = RC[i]->cread();
						//cout<<"the response was: "<<response<<endl;
						//cout<<"response put in: ";
						int ind = identify(request);
						if(ind == 0){
							joeBuffer.add(response);
							response_count++;
							//cout<<"Joe Buffer"<<endl;
						}	
						else if(ind == 1){
							janeBuffer.add(response);
							response_count++;
							//cout<<"Jane Buffer"<<endl;
						}
						else if(ind == 2){
							johnBuffer.add(response);
							response_count++;
							//cout<<"John Buffer"<<endl;
						}
						else cout<<"DEFAULT CASE????"<<endl;//not necessary
					//response_count++;
					}
					
					
					if(removed_count < 3*n+n/2)
					{//funky boundary condition
						request_name[i] = RequestBuffer.remove();
						RC[i]->cwrite(request_name[i]);//not sure about this one
						removed_count++;
						//cout<<"response_count: "<<response_count<<endl;
						//cout<<"removed_count: "<<removed_count<<endl;
					}
				}
			}
		}
		if(/*removed_count > 3*n+n/2 /*&&*/ response_count >= 3*n) {//funky boundary condition
			//cout<<"removed count: "<<removed_count<<endl; 
			//cout<<"response count: "<<response_count<<endl;
			break; 
			
		}
		// Close channels
	    
    }
    //cout<<"n: "<<n<<endl;
    //cout<<"3*n+n/2: "<<3*n+n/2<<endl;
    //cout<<"removed count after infinite while loop: "<<removed_count<<endl;
    if(removed_count < 3*n+n/2 - 1){
    	while(removed_count < 3*n+n/2 - 1){
    		RequestBuffer.remove();
    		removed_count++;	
    	}
    }
    //cout<<"removed count on exit: "<<removed_count<<endl;
	for(int i = 0; i <w; i++){
	        RC[i]->cwrite("quit");
	        //usleep(30000);//probably not necessary	//i agree
	        RC[i]->cread();
	        
	}
	
	//usleep(30000);//i don't think this is necessary //nope
	//cout<<"broke out of while loop~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
	//cout<<"termination"<<endl;
	//sleep(3);
/*	// Close channels
    for(int i = 0; i <w; i++){
        RC[i]->send_request("Stop");
    }
    rChan->send_request("Stop");*/
	
}



int main(int argc, char * argv[]) {

	int opt;
	int get_n, get_b, get_w;//read in values for n,b,w
	string get_h; 
	unsigned short get_p;
	while((opt = getopt(argc, argv, "n::b::w::h::p::")) != -1){
		
		switch (opt){
			case 'n':{
				if(argv[2]) get_n = atoi(argv[2]);
				else get_n=100000;
				//printf("b: %s \n",argv[2]);
				break;
			}
			case 'b':{
				if(argv[4]) get_b = atoi(argv[4]);
				else get_b=100;
				//printf("b: %s \n",argv[2]);
				break;
			}
			case 'w':{
				if(argv[6]) get_w = atoi(argv[6]);
				else get_w=40;
				//printf("b: %s \n",argv[2]);
				break;
			}
			case 'h':{
				if(argv[8]) get_h = string( argv[8]);
				else get_h="www.example.com";
				//printf("b: %s \n",argv[2]);
				break;
			}
			case 'p':{
				if(argv[10]) get_p = atoi(argv[10]);
				else get_p=80;
				//printf("b: %s \n",argv[2]);
				break;
			}
			default:{
				fprintf(stderr, "Usage: %s [-n get_n] [-b get_b] [-w get_w]\n",argv[0]);
				exit(EXIT_FAILURE);
			}
		}	
	}
	printf("using values: n = %d \n b = %d \n w = %d", get_n, get_b, get_w);
	//printf("connecting to: %s:%s", get_h, get_p);
	cout<<"connecting to: "<<get_h<<":"<<get_p<<endl;
	host = get_h;
	port = get_p;

	
		cout << "CLIENT STARTED:" << endl;

		cout << "Establishing control channel... " << flush;
		NetworkRequestChannel chan(host, port);
		cout << "done." << endl;;

	  /* -- Start sending a sequence of requests */
		// pthread_t tid;
		int index[] = {0, 1, 2};
		
		/* -- assigning passed in values -- */
		b = get_b;
		n = get_n;
		w = get_w;
		// w = 40;//number of worker threads
		// n = 10000;//number of requests per person
		// b = 100;//size of bounded buffer
		//identify("data Jane Smith");
		
		/* timing */
		timeval begin1, end1;
		gettimeofday(&begin1,NULL);
		
		
		
		// -- request threads --
		pthread_t tid1;
		pthread_t tid2;
		pthread_t tid3;
		
		// -- event handler thread --
		pthread_t eid;
		
		// -- stat threads --
		pthread_t stid1;
		pthread_t stid2;
		pthread_t stid3;
		
		//buffer = new BoundedBuffer(buffer_size);//instance of *buffer
		RequestBuffer = BoundedBuffer(b);//instance of bounded buffer of size b
		
		// -- creating worker threads --
		pthread_create(&tid1,NULL,RTF,&index[0]);
		pthread_create(&tid2,NULL,RTF,&index[1]);
		pthread_create(&tid3,NULL,RTF,&index[2]);
		
		/*
		for(int i=0;i<w;i++){
			string ch = chan.send_request("newthread");
			RequestChannel* newchan = new RequestChannel(ch,RequestChannel::CLIENT_SIDE);
			pthread_create(&wtid[i],NULL,WTF,newchan);
		}
		*/
		//usleep(2000000);
		
		
 		
 		cout<<"creating even handler threads."<<endl;
		// -- create event handler thread --
		pthread_create(&eid,NULL,EventHandler,&chan);
		
		
		
		
		// -- create stat threads --
		pthread_create(&stid1,NULL,statThread,&index[0]);
		pthread_create(&stid2,NULL,statThread,&index[1]);
		pthread_create(&stid3,NULL,statThread,&index[2]);
		
		
		// -- join request threads --
		pthread_join(tid1,NULL);
		pthread_join(tid2,NULL);
		pthread_join(tid3,NULL);
		
		cout<<"sending termination signals."<<endl;
		// -- join event handler thread --
		for(int i=0;i<n/2;i++){//push n/2 Stop signals 
			RequestBuffer.add("Stop");
		}
		cout<<"joining thread ++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
		//cout<<"pthread: "<<
		pthread_join(eid,NULL);
		
		cout<<"joined event handler thread~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
		
		// -- join stat threads --
		pthread_join(stid1,NULL);
		pthread_join(stid2,NULL);
		pthread_join(stid3,NULL);
		
		//cout<<"joined stat threads"<<endl;
		chan.send_request("quit");
		gettimeofday(&end1,NULL);
		//long time_taken = end1.tv_usec - begin1.tv_usec;
		//cout<<"\n\n\n\n\n\n\n\nTotal time taken: "<<((long)(end1.tv_sec) + (long)end1.tv_usec/1000000) - ((long)(begin1.tv_sec) + (long)begin1.tv_usec/1000000);
		//cout<<"\n\n\n\n\n\n\n\nTotal time taken: "<<((long)(end1.tv_sec*1000000) + (long)end1.tv_usec) - ((long)(begin1.tv_sec*1000000) + (long)begin1.tv_usec);
		//cout<<"\n\n\n\n\n\n\n\nTotal time taken: "<<time_taken;
		
		
		sleep(1);
		cout<<"\n*********************\nFinal Output:\n*********************"<<endl;
		display(joeGraph,"joe smith");
		display(janeGraph,"jane smith");
		display(johnGraph,"john doe");
		
		
		cout<<"Total time taken: "<<((long)(end1.tv_sec*1000000) + (long)end1.tv_usec) - ((long)(begin1.tv_sec*1000000) + (long)begin1.tv_usec)<<" us";
		cout<<endl;
		// for (int i = 0; i<3;i++){
			// pthread_create(&tid[i],NULL,RTF,&index[i]);
		// }
		//pthread_create(&tid, NULL, , );
		
	  /* string reply1 = chan.send_request("hello");
	  cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;
	  string reply2 = chan.send_request("data Joe Smith");
	  cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;
	  string reply3 = chan.send_request("data Jane Smith");
	  cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;
	  string reply5 = chan.send_request("newthread");
	  cout << "Reply to request 'newthread' is " << reply5 << "'" << endl;
	  RequestChannel chan2(reply5, RequestChannel::CLIENT_SIDE);
	  string reply6 = chan2.send_request("data John Doe");
	  cout << "Reply to request 'data John Doe' is '" << reply6 << "'" << endl;
	  string reply7 = chan2.send_request("quit");
	  cout << "Reply to request 'quit' is '" << reply7 << "'" << endl;
	  string reply4 = chan.send_request("quit");
	  cout << "Reply to request 'quit' is '" << reply4 << "'" << endl;
	  */
	  //usleep(1000000); 
	
}

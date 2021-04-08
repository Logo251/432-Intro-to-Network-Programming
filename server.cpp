#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include    <netdb.h>     // gethostbyname
#include    <unistd.h>    // read, write, close
#include   <strings.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <sys/time.h>     //gettimeofday
#include <string>         //string
#include <pthread.h>      //Pthread
#include <iostream>

using namespace std;

//Global Constant
const int BUFFERSIZE = INT32_MAX;

//Function Prototype
void* ConnectionProcessor(void* ptr);

//Structure
struct connectionProcessorData {
    sockaddr_in socket;
    int repetition;
};

int main(int argc, char* argv[]) {
    //Local Variables
    int port = (int)*argv[0];
    int repetition = (int)*argv[1];
    sockaddr_in acceptSock;
    const int on = 1;
    pthread_t thread;
    sockaddr_in newsock;

    bzero((char*) &acceptSock, sizeof(acceptSock));  // zero out the data structure
    acceptSock.sin_family = AF_INET;   // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(port);  // set the port to listen on

    int serverSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP

    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));  // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket
    bind(serverSd, (sockaddr*) &acceptSock, sizeof(acceptSock));  // bind the socket using the parameters we set earlier

    // Listen on the socket
    int n = 8;
    listen(serverSd, n);  // listen on the socket and allow up to n connections to wait.

    // Accept the connection as a new socket
    socklen_t newsockSize = sizeof(newsock);

    while (1) {
        //int newSd = accept(serverSd, (sockaddr *)newsock &newsockSize);  // grabs the new connection and assigns it a temporary socket

        //Create the structure here, so each thread has its own structure.
        connectionProcessorData* structure = new connectionProcessorData;
        structure->repetition = repetition;
        structure->socket = newsock; //TODO: improve name.

        //Send the input to a thread to deal with it
        pthread_create(&thread, NULL, ConnectionProcessor, &structure);
        // Close the socket
        //close(newSd); //TODO: MOVE/REMOVE THIS

    }
    return 0;
}

void* ConnectionProcessor(void* ptr) {
    //Local Variables
    timeval startTime;
    timeval endTime;

    //Allocate databuf[BUFSIZE]
    char databuf[BUFFERSIZE];

    //Start a timer by calling gettimeofday.
    gettimeofday(&startTime, NULL);

    //Repeat reading data from the client into databuf[BUFSIZE].

    //Stop the timer by calling gettimeofday.
    gettimeofday(&endTime, NULL);

    //Send the number of read( ) calls made.
    //write(sd, databuf, nbufs * bufsize);

    //Print out the statistics.
    //EXAMPLE:   data-receiving time = xxx usec

    //Close this connection.

    //Clean up memory.
    delete ptr;

    return 0;
}
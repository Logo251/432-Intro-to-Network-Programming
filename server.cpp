#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
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
const int BUFFERSIZE = 1500;

//Function Prototype
void* ConnectionProcessor(void* input);

//Structure
struct connectionProcessorData {
    int socket;
    int repetition;
};

int main(int argc, char* argv[]) {
    //Local Variables
    int port = atoi(argv[1]);
    int repetition = atoi(argv[2]);
    sockaddr_in acceptSock;
    const int on = 1;
    sockaddr_in newsock;
    int totalConnections = 8;

    bzero((char*) &acceptSock, sizeof(acceptSock));  // zero out the data structure
    acceptSock.sin_family = AF_INET;   // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(port);  // set the port to listen on

    int serverSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP

    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));  // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket
    bind(serverSd, (sockaddr*) &acceptSock, sizeof(acceptSock));  // bind the socket using the parameters we set earlier

    // Listen on the socket
    listen(serverSd, totalConnections);  // listen on the socket and allow up to n connections to wait.
    socklen_t newsockSize = sizeof(newsock);

    std::cout << "Beginning to watch for incoming!\n";

    while (true) {
        // grabs the new connection and assigns it a temporary socket
        int newSd = accept(serverSd, (sockaddr *)&newsock, &newsockSize);

        //Create new thread
        pthread_t thread;

        //Create the structure here, so each thread has its own structure.
        connectionProcessorData* threadData = new connectionProcessorData;
        threadData->repetition = repetition;
        threadData->socket = newSd;

        //Send the input to a thread to deal with it
        pthread_create(&thread, nullptr, ConnectionProcessor, (void*) threadData);
    }
    return 0;
}

void* ConnectionProcessor(void* input) {
    //Local Variables
    timeval startTime;
    timeval endTime;
    connectionProcessorData* data = (connectionProcessorData*)(input);
    int count = 0;

    //Allocate databuf[BUFSIZE]
    char databuf[BUFFERSIZE];

    //Start a timer by calling gettimeofday.
    gettimeofday(&startTime, NULL);

    //Repeat reading data from the client into databuf[BUFSIZE].
    for(int i =0; i < data->repetition; i++) {
        for ( int nRead = 0;
              (nRead += read(data->socket, databuf, BUFFERSIZE - nRead)) < BUFFERSIZE;
              ++count );
        count++;
    }

    //Stop the timer by calling gettimeofday.
    gettimeofday(&endTime, NULL);

    //Send the number of read( ) calls made.
    write(data->socket, to_string(count).c_str(), BUFFERSIZE);

    //Print out the statistics.
    timersub(&endTime, &startTime, &endTime);
    std::cout << "data-receiving time = " + to_string(endTime.tv_usec) + " usec";
    cout.flush();

    //Close this connection.
    close(data->socket);

    //Clean up memory.
    delete data;

    return 0;
}
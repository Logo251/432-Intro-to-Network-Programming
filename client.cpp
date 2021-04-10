#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include    <netdb.h>     // gethostbyname
#include    <unistd.h>    // read, write, close
#include   <strings.h>     // bzero
#include <sys/uio.h>      // writev
#include <sys/time.h>     //gettimeofday
#include <string>         //string
#include <iostream>

using namespace std;

const int BUFFERSIZE = 1500;

int main(int argc, char* argv[]) {
    //Local Variables
    //Variables from user input.
    int port = atoi(argv[1]);
    int repetition = atoi(argv[2]);
    int nbufs = atoi(argv[3]);
    int bufsize = atoi(argv[4]);
    char* serverIp = argv[5];
    int type = atoi(argv[6]);

    //Other Variables
    timeval startTime;
    timeval lapTime;
    timeval sendTime;
    char recievebuf[BUFFERSIZE] = { 0 };
    sockaddr_in sendSockAddr;
    struct hostent* host = gethostbyname(serverIp);

    //-----------------------------------------------------
    //Open new socket and establish connection to a server.
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET; // Address Family Internet
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

    int sd = socket( AF_INET, SOCK_STREAM,0 ); // creates a new socket for IP using TCP

    connect(sd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));

    //---------------------------------
    //Allocate databuf[nbufs][bufsize].
    char databuf[nbufs][bufsize];

    //Put something unique in buffer.
    srand (time(nullptr));
    int start = rand() % 10;
    for(int i = 0; i < nbufs; i++) {
        for(int j = 0; j < bufsize; j++) {
            databuf[i][j] = start;
            ++start;
        }
    }

    //--------------------------------------
    //Start a timer by calling gettimeofday.
    gettimeofday(&startTime, NULL);

    //-----------------------------------------------------------------
    //Repeat the repetition times of data transfers, each based
    //on type such as 1: multiple writes, 2: writev, or 3: single write

    //Type 1, multiple writes.
    if(type == 1) {
        for(int i = 0; i < repetition; i++) {
            for ( int j = 0; j < nbufs; j++ )
                write( sd, databuf[j], bufsize);
        }
    }
    //Type 2, writev
    else if(type == 2) {
        struct iovec vector[nbufs];
        for(int i = 0; i < repetition; i++) {
            for (int j = 0; j < nbufs; j++) {
                vector[j].iov_base = databuf[j];
                vector[j].iov_len = bufsize;
            }
            writev(sd, vector, nbufs);           // sd: socket descriptor
        }
    }
    //Type 3, single write
    else {
        for(int i = 0; i < repetition; i++) {
            write(sd, databuf, nbufs * bufsize); // sd: socket descriptor
        }
    }

    //-----------------------------------------------------------------------------
    //Lap the timer by calling gettimeofday, where lap - start = data-sending time.
    gettimeofday(&lapTime, NULL);


    //-------------------------------------------------------------------------------------------------------
    //Receive from the server an integer acknowledgement that shows how many times the server called read().
    read(sd, recievebuf, BUFFERSIZE);

    //-----------------------------------------------------------------------------
    //Stop the timer by calling gettimeofday, where stop - start = round-trip time.
    gettimeofday(&sendTime, NULL);

    //-------------------------
    //Print out the statistics.
    //EXAMPLE: Test 1: data-sending time = xxx usec, round-trip time = yyy usec, #reads = zzz
    timersub(&lapTime, &startTime, &lapTime);
    timersub(&sendTime, &startTime, &sendTime);
    std::cout << "Test " + to_string(type) + ": data-sending time = " + to_string(lapTime.tv_usec) + " usec, round-trip time = " + to_string(sendTime.tv_usec) + " usec, #reads = " + recievebuf;

    //-------------
    //Close socket.
    close(sd);

    return 0;
}

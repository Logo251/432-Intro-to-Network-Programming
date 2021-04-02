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
using namespace std;

int main(int argc, char* argv[]) {
    //Local Variables
    //Variables from user input.
    int port = (int)*argv[0];
    std::string repetition = to_string(*argv[1]);
    int nbufs = (int)*argv[2];
    int bufsize = (int)*argv[3];
    char* serverIp = argv[4];
    int type = (int)*argv[5];

    //Other Variables
    timeval startTime;
    timeval lapTime;
    timeval sendTime;

    //-----------------------------------------------------
    //Open new socket and establish connection to a server.
    struct hostent* host = gethostbyname(serverIp);

    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET; // Address Family Internet
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

    int sd = socket( AF_INET, SOCK_STREAM,0 ); // creates a new socket for IP using TCP

    connect(sd, (sockaddr*)&sendSock, sizeof(sendSock));

    //---------------------------------
    //Allocate databuf[nbufs][bufsize].

    //--------------------------------------
    //Start a timer by calling gettimeofday.
    gettimeofday(&startTime, NULL);

    //-----------------------------------------------------------------
    //Repeat the repetition times of data transfers, each based
    //on type such as 1: multiple writes, 2: writev, or 3: single write

    //Type 1, multiple writes.
    if(type == 1) {
        for ( int j = 0; j < nbufs; j++ )
            write( sd, databuf[j], bufsize );    // sd: socket descriptor
    }
    //Type 2, writev
    else if(type == 2) {
        struct iovec vector[nbufs];
        for ( int j = 0; j < nbufs; j++ ) {
            vector[j].iov_base = databuf[j];
            vector[j].iov_len = bufsize;
        }
        writev( sd, vector, nbufs );           // sd: socket descriptor
    }
    //Type 3, single write
    else {
        write(sd, databuf, nbufs * bufsize); // sd: socket descriptor
    }

    //-----------------------------------------------------------------------------
    //Lap the timer by calling gettimeofday, where lap - start = data-sending time.
    gettimeofday(&lapTime, NULL);
    timersub(&lapTime, &startTime, &lapTime);

    //-------------------------------------------------------------------------------------------------------
    //Receive from the server an integer acknowledgement that shows how many times the server called read().


    //-----------------------------------------------------------------------------
    //Stop the timer by calling gettimeofday, where stop - start = round-trip time.
    gettimeofday(&sendTime, NULL);
    timersub(&sendTime, &startTime, &sendTime);

    //-------------------------
    //Print out the statistics.
    //EXAMPLE: Test 1: data-sending time = xxx usec, round-trip time = yyy usec, #reads = zzz

    //-------------
    //Close socket.
    close(sd);

    return 0;
}

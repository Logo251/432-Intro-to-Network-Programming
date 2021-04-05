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
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    //Local Variables
    int port = (int)*argv[0];
    int repetition = (int)*argv[1];
    sockaddr_in acceptSock;
    const int on = 1;

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
    sockaddr_in newsock;   // place to store parameters for the new connection
    socklen_t newsockSize = sizeof(newsock);

    while (1) {
        int newSd = accept(serverSd, (sockaddr *)&newsock, &newsockSize);  // grabs the new connection and assigns it a temporary socket

        //Send the input to a thread to deal with it
        pthread_create(connectionProcessor(newsock));

        // Close the socket
        close(newSd);
    }
    return 0;
}

void ConnectionProcessor(sockaddr_in socket) {

}
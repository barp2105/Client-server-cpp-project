/*
Final cpp project by: 
Bar Partush - 315431221  , 
Amit Kalaf  - 208311654
*/


// Includes libraries and dependencies

#include "../std_lib_facilities.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]){
    // Create a TCP socket
    int fd = socket(AF_INET,SOCK_STREAM,0);

    // Define server address structure
    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]); // Server IP address from command line
    addr.sin_port = htons(atoi(argv[2]));   // Server port from command line

    // Connect to the server using the socket and server address
    connect(fd, (sockaddr*)&addr , sizeof(addr));

    // Prepare request data from command line arguments
    string start = argv[3];
    string end = argv[4];
    string ServerRequest = start + " " + end;

    // Send the request to the server
    write(fd, ServerRequest.c_str() , ServerRequest.length());

    // Buffer to store server response
    char Response[1024];
    memset(Response, 0, sizeof(Response));

    // Read the server response
    read(fd,Response, sizeof(Response)-1);
    Response[sizeof(Response)-1]='\0';

    // Output the server response to standard output
    cout << Response;
    
    // Close the socket
    close(fd);
    return 0;
}